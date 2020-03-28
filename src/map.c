#include <libgen.h>
#include <string.h>

#include <jansson.h>

#include "log.h"
#include "map.h"


struct map* map_load(const char* path){
    json_error_t error;
    json_t* root = json_load_file(path, JSON_REJECT_DUPLICATES, &error);
    if(root == NULL){
        logmsg(LOG_ERR, "Could not load map '%s', %s at line %d, column %d", path, error.text, error.line, error.column);

        return NULL;
    }

    json_t* layers = NULL; 
    json_t* tilesets = NULL;

    struct map* map = calloc(1, sizeof(struct map));
    if(map == NULL){
        logmsg(LOG_ERR, "Could not load map '%s', the system is out of memory", path);

        goto fail_map;
    }

    int ret = json_unpack_ex( root, 
                              &error, 
                              0, 
                              "{s:i, s:i, s:i, s:i, s:o, s:o, s:F}", 
                              "height", &map->height, 
                              "width", &map->width, 
                              "tileheight", &map->tile_height, 
                              "tilewidth", &map->tile_width, 
                              "layers", &layers,
                              "tilesets", &tilesets,
                              "version", &map->version
                            );

    if(ret == -1){
        logmsg(LOG_ERR, "Could not unpack map '%s', %s", path, error.text);

        goto fail_map;
    }

    logmsg(LOG_DEBUG, "Unpacked root object for map '%s'", path);

    // Load layers
    if(!json_is_array(layers)){
        logmsg(LOG_ERR, "Could not unpack map '%s', 'layers' must be an array");

        goto fail_map;
    }

    size_t layer_count = json_array_size(layers);

    if(layer_count < 1){
        logmsg(LOG_ERR, "Unable to unpack map '%s', 'layers' array must have at least one element");

        goto fail_map;
    }

    map->layer_count = layer_count;

    map->layers = calloc(layer_count, sizeof(struct layer));
    if(map->layers == NULL){
        logmsg(LOG_ERR, "Could not load layers for map '%s', the system is out of memory", path);

        goto fail_map;
    }

    size_t index;
    json_t* value;

    json_array_foreach(layers, index, value){
        char* type = NULL;
        json_t* data = NULL;

        ret = json_unpack_ex( value, 
                              &error,
                              0,
                              "{s:i, s:i, s?F, s:s, s?F, s?F, s:o, s:s}", 
                              "height", &map->layers[index].height,
                              "width", &map->layers[index].width,
                              "opacity", &map->layers[index].opacity,
                              "type", &type,
                              "offsetx", &map->layers[index].offset_x,
                              "offsety", &map->layers[index].offset_y,
                              "data", &data,
                              "name", &map->layers[index].name
                            );

        if(ret == -1){
            logmsg(LOG_ERR, "Could not load layer for map '%s', %s", path, error.text);

            goto fail_layers;
        }

        logmsg(LOG_DEBUG, "Unpacked root object for layer '%s' in map '%s'", map->layers[index].name, path);

        if(!json_is_array(data)){
            logmsg(LOG_ERR, "Could not load layer for map '%s', layer data must be an array", path);

            goto fail_layers;
        }

        if(strcmp(type, "tilelayer") == 0){
            map->layers[index].type = TILE;
        }
        else if(strcmp(type, "objectlayer") == 0){
            map->layers[index].type = OBJECT;
        }
        else{
            logmsg(LOG_ERR, "Could not load layers for map '%s', unknown layer type '%s'", path, type);

            goto fail_layers;
        }

        map->layers[index].data = calloc(json_array_size(data), sizeof(int));
        if(map->layers[index].data == NULL){
            logmsg(LOG_ERR, "Could not load layer data for map '%s', the system is out of memory", path);

            for(size_t i = 0; i <= index; i++){
                free(map->layers[i].data);
            }

            goto fail_layers;
        }

        size_t data_index;
        json_t* data_value;

        json_array_foreach(data, data_index, data_value){
            ret = json_unpack_ex(data_value, &error, 0, "i", &map->layers[index].data[data_index]);
            if(ret == -1){
                logmsg(LOG_ERR, "Could not load layer data for map '%s', %s", path, error.text);

                for(size_t i = 0; i <= index; i++){
                    free(map->layers[i].data);
                }

                goto fail_layers;
            }
        }
    }

    // Load tilesets
    if(!json_is_array(tilesets)){
        logmsg(LOG_ERR, "Could not unpack map '%s', 'tilesets' must be an array");

        goto fail_tilesets;
    }

    size_t tileset_count = json_array_size(tilesets);

    if(tileset_count < 1){
        logmsg(LOG_ERR, "Unable to unpack map '%s', 'tilesets' array must have at least one element");

        goto fail_tilesets;
    }

    map->tileset_count = tileset_count;

    map->tilesets = calloc(tileset_count, sizeof(struct tileset*));
    if(map->tilesets == NULL){
        logmsg(LOG_ERR, "Could not allocate space for tilesets, the system is out of memory");

        goto fail_tilesets;
    }

    json_array_foreach(tilesets, index, value){
        char* type = NULL;
        char* source = NULL;

        ret = json_unpack_ex(value, &error, 0, "{s:s}", "source", &source);

        if(ret == -1){
            logmsg(LOG_ERR, "Could not unpack tileset source for map '%s', %s", path, error.text);

            goto fail_tilesets;
        }

        // The tileset path is relative to the map path, so we need to build it now
        char* path_copy = strdup(path);

        char* dir = dirname(path_copy);

        char* tileset_path = calloc(1, strlen(dir) + strlen("/") + strlen(source));

        char* to = tileset_path;

        to = stpcpy(to, dir);
        to = stpcpy(to, "/");
        to = stpcpy(to, source);

        free(path_copy);

        map->tilesets[index] = tileset_load(tileset_path);
        if(map->tilesets[index] == NULL){
            logmsg(LOG_ERR, "Could not load tileset '%s'", tileset_path);

            //Free any tilesets you've created so far, then free the tileset pointer array.

            goto fail_tilesets;
        }
    }

    return map;


fail_tilesets:
    for(size_t i = 0; i < map->layer_count; i++){
        free(map->layers[i].data);
    }

fail_layers:
    free(map->layers);

fail_map:
    json_decref(root);

    free(map);

    return NULL;
}

struct tileset* tileset_load(const char* path){
    json_error_t error;

    json_t* root = json_load_file(path, JSON_REJECT_DUPLICATES, &error);
    if(root == NULL){
        logmsg(LOG_ERR, "Unable to load tileset '%s', %s at line %d, column %d", path, error.text, error.line, error.column);

        return NULL;
    }

    struct tileset* ts = calloc(1, sizeof(struct tileset));
    if(ts == NULL){
        logmsg(LOG_ERR, "Unable to load tileset '%s', the system is out of memory", path);
        
        goto fail_tileset;
    }

    // Determine whether we need to load a sheet or a set of individual tile images
    char* image_path = NULL;

    int ret = json_unpack_ex(root, &error, 0, "s?s, s:F", "image", &image_path, "version", &ts->version);
    if(ret == -1){
        logmsg(LOG_ERR, "Unable to unpack tileset '%s', %s", path, error.text);

        goto fail_tileset;
    }

    // If there's an "image" field in the root object, this is a sheet,
    // otherwise there's a "tiles" array with individual tile objects in it.
    if(image_path == NULL){
        ts->type = INDIVIDUAL;

        json_t* tiles = NULL;

        ret = json_unpack_ex(root, &error, 0, "s:o", "tiles", &tiles);
        if(ret == -1){
            logmsg(LOG_ERR, "Unable to unpack tileset '%s', %s", path, error.text);

            goto fail_tileset;
        }

        if(!json_is_array(tiles)){
            logmsg(LOG_ERR, "Unable to unpack tileset '%s', 'tiles' must be an array", path);

            goto fail_tileset;
        }

        size_t tile_count = json_array_size(tiles);

        if(tile_count < 1){
            logmsg(LOG_ERR, "Unable to unpack tileset '%s', 'tiles' array must have at least one element", path);

            goto fail_tileset;
        }

        ts->tiles = calloc(tile_count, sizeof(struct tile));
        if(ts->tiles == NULL){
            logmsg(LOG_ERR, "Unable to unpack tileset '%s', the system is out of memory", path);

            goto fail_tileset;
        }

        size_t index;
        json_t* value;
        json_array_foreach(tiles, index, value){
            ret = json_unpack_ex(   root,
                                    &error,
                                    0,
                                    "{s:i, s:s, s:i, s:i}",
                                    "id", &ts->tiles[index].id,
                                    "image", &ts->tiles[index].path,
                                    "imageheight", &ts->tiles[index].height_px,
                                    "imagewidth", &ts->tiles[index].width_px
                                );

            if(ret == -1){
                logmsg(LOG_ERR, "Unable to unpack tileset '%s', %s", path, error.text);

                free(ts->tiles);

                goto fail_tileset;
            }

            //Either memory-map the file and save pointer in
            //ts->tiles[index].data, or use SDL to load a surface directly, and
            //save that in the tile object
        }
    }
    else{
        ts->type = SHEET;
        
        struct sheet* sheet = calloc(1, sizeof(struct sheet));
        if(sheet == NULL){
            logmsg(LOG_ERR, "Unable to load tileset '%s', the system is out of memory", path);

            goto fail_tileset;
        }
    }

fail_tileset:
    json_decref(root);

    free(ts);

    return NULL;
}
