#include <libgen.h>
#include <string.h>
#include <unistd.h>

#include <jansson.h>

#include "log.h"
#include "tmj.h"


Property* unpack_properties(json_t* properties){
    if(properties == NULL){
        return NULL;
    }

    if(!json_is_array(properties)){
        logmsg(LOG_ERR, "'properties' must be an array");

        return NULL;
    }

    json_error_t error;

    size_t property_count = json_array_size(properties);

    Property* ret = calloc(property_count, sizeof(Property));

    if(ret == NULL){
        logmsg(LOG_ERR, "Unable to unpack properties, the system is out of memory");

        return NULL;
    }

    size_t idx;
    json_t* property;

    json_array_foreach(properties, idx, property){
        int unpk = json_unpack_ex(property,
                                  &error,
                                  0,
                                  "{s:s, s?s, s?s, s:o}",
                                  "name", &ret[idx].name,
                                  "type", &ret[idx].type,
                                  "propertytype", &ret[idx].propertytype,
                                  "value", &ret[idx].value
                                 );

        if(unpk == -1){
            logmsg(LOG_ERR, "Unable to unpack properties, %s at line %d column %d", error.text, error.line, error.column);

            free(ret);

            return NULL;
        }
    }

    return ret;
}

Tileset* load_tileset(){
    return NULL;
}

Point* unpack_points(json_t* points){
    if(points == NULL){
        return NULL;
    }

    if(!json_is_array(points)){
        logmsg(LOG_ERR, "'points' must be an array");

        return NULL;
    }

    json_error_t error;

    size_t point_count = json_array_size(points);

    Point* ret = calloc(point_count, sizeof(Point));

    if(ret == NULL){
        logmsg(LOG_ERR, "Unable to unpack points, the system is out of memory");

        return NULL;
    }

    size_t idx;
    json_t* point;

    json_array_foreach(points, idx, point){
        int unpk = json_unpack_ex(point,
                                  &error,
                                  0,
                                  "{s:F, s:F}",
                                  "x", &ret[idx].x,
                                  "y", &ret[idx].y
                                 );

        if(unpk == -1){
            logmsg(LOG_ERR, "Unable to unpack points, %s at line %d column %d", error.text, error.line, error.column);

            free(ret);

            return NULL;
        }
    }

    return ret;
}

Text* unpack_text(json_t* text){
    if(text == NULL){
        return NULL;
    }

    json_error_t error;

    Text* ret = calloc(1, sizeof(Text));

    if(ret == NULL){
        logmsg(LOG_ERR, "Unable to unpack text, the system is out of memory");

        return NULL;
    }

    int unpk = json_unpack_ex(text,
                              &error,
                              0,
                              "{"
                              "s?b, s?b, s?b, s?b, s?b, s?b,"
                              "s?s, s?s?, s?s, s:s, s?s,"
                              "s?i,"
                              "}",
                              "bold", &ret->bold,
                              "italic", &ret->italic,
                              "kerning", &ret->kerning,
                              "strikeout", &ret->strikeout,
                              "underline", &ret->underline,
                              "wrap", &ret->wrap,
                              "color", &ret->color,
                              "fontfamily", &ret->fontfamily,
                              "halign", &ret->halign,
                              "text", &ret->text,
                              "valign", &ret->valign,
                              "pixelsize", &ret->pixelsize
                             );
    if(unpk == -1){
        logmsg(LOG_ERR, "Unable to unpack text, %s at line %d column %d", error.text, error.line, error.column);

        free(ret);

        return NULL;
    }

    return ret;
}

Object* unpack_objects(json_t* objects){
    if(objects == NULL){
        return NULL;
    }

    if(!json_is_array(objects)){
        logmsg(LOG_ERR, "'objects' must be an array");
    }

    json_error_t error;

    size_t object_count = json_array_size(objects);

    Object* ret = calloc(object_count, sizeof(Object));

    if(ret == NULL){
        logmsg(LOG_ERR, "Unable to unpack objects, the system is out of memory");

        return NULL;
    }

    size_t idx;
    json_t* object;

    json_array_foreach(objects, idx, object){
        //Unpack scalar values
        int unpk = json_unpack_ex(object,
                                  &error,
                                  0,
                                  "{"
                                  "s:b, s:b, s:b,"
                                  "s?s, s:s, s:s,"
                                  "s?i, s:i,"
                                  "s:F, s:F, s:F, s:F, s:F,"
                                  "}",
                                  "ellipse", &ret[idx].ellipse,
                                  "point", &ret[idx].point,
                                  "visible", &ret[idx].visible,
                                  "class", &ret[idx].class,
                                  "name", &ret[idx].name,
                                  "template", &ret[idx].template,
                                  "gid", &ret[idx].gid,
                                  "id", &ret[idx].id,
                                  "height", &ret[idx].height,
                                  "rotation", &ret[idx].rotation,
                                  "width", &ret[idx].width,
                                  "x", &ret[idx].x,
                                  "y", &ret[idx].y
                                 );
        
        if(unpk == -1){
            logmsg(LOG_ERR, "Unable to unpack object, %s at line %d column %d", error.text, error.line, error.column);

            return NULL;
        }

        //Unpack properties
        json_t* properties = NULL;

        unpk = json_unpack_ex(object, &error, 0, "{s?o}", "properties", &properties);

        if(unpk == -1){
            logmsg(LOG_ERR, "Unable to unpack object, %s at line %d column %d", error.text, error.line, error.column);

            goto fail_properties;
        }

        if(properties != NULL){
            ret[idx].properties = unpack_properties(properties);

            if(ret[idx].properties == NULL){
                logmsg(LOG_ERR, "Unable to unpack object properties");

                goto fail_properties;
            }

            ret[idx].property_count = json_array_size(properties);
        }

        // Unpack text
        json_t* text = NULL;

        unpk = json_unpack_ex(object, &error, 0, "{s?o}", "text", &text);

        if(unpk == -1){
            logmsg(LOG_ERR, "Unable to unpack object, %s at line %d column %d", error.text, error.line, error.column);

            goto fail_text;
        }

        if(text != NULL){
            ret[idx].text = unpack_text(text);

            if(ret[idx].text == NULL){
                logmsg(LOG_ERR, "Unable to unpack object text");

                goto fail_text;
            }
        }

        // If this object is any of the below items, we don't need to unpack anything else
        if(ret[idx].ellipse || ret[idx].point || ret[idx].gid != 0 || ret[idx].text != NULL){
            continue;
        }

        // Unpack Polygon
        json_t* polygon = NULL;

        unpk = json_unpack_ex(object, &error, 0, "{s?o}", "polygon", &polygon);

        if(unpk == -1){
            logmsg(LOG_ERR, "Unable to unpack object, %s at line %d column %d", error.text, error.line, error.column);

            goto fail_polygon;
        }

        if(polygon != NULL){
            ret[idx].polygon = unpack_points(polygon);
            
            if(ret[idx].polygon == NULL){
                goto fail_polygon;
            }

            ret[idx].polygon_point_count == json_array_size(polygon);

            // No need to unpack a polyline if this object was a polygon
            continue;
        }

        // Unpack Polyline
        json_t* polyline = NULL;

        unpk = json_unpack_ex(object, &error, 0, "{s?o}", "polyline", &polyline);

        if(unpk == -1){
            logmsg(LOG_ERR, "Unable to unpack object, %s at line %d column %d", error.text, error.line, error.column);

            goto fail_polyline;
        }

        if(polyline != NULL){
            ret[idx].polyline = unpack_points(polyline);

            if(ret[idx].polyline == NULL){
                goto fail_polyline;
            }

            ret[idx].polyline_point_count == json_array_size(polyline);
        }
    }

    return ret;

fail_polygon:
fail_polyline:
    for(size_t i = 0; i < object_count; i++){
        free(ret[i].text);
    }

fail_text:
    for(size_t i = 0; i < object_count; i++){
        free(ret[i].properties);
    }

fail_properties:
    free(ret);

    return NULL;
}

/**
 * Loads map layers recursively
 */
Layer* unpack_layers(json_t* layers){
    if(!json_is_array(layers)){
        logmsg(LOG_ERR, "Could not unpack layer, 'layers' must be an array");

        return NULL;
    }

    size_t layer_count = json_array_size(layers);

    if(layer_count < 1){
        logmsg(LOG_ERR, "Unable to load layers, 'layers' array must have at least one element");

        return NULL;
    }

    Layer* ret = calloc(layer_count, sizeof(Layer));

    if(ret == NULL){
        logmsg(LOG_ERR, "Unable to load layers, the system is out of memory");

        return NULL;
    }

    size_t idx;
    json_t* layer;
    json_error_t error;

    json_array_foreach(layers, idx, layer){
        // Unpack id so we can log errors with it
        int unpk = json_unpack_ex(layer, &error, 0, "{s:i}", "id", &ret[idx].id);

        if(unpk == -1){
            logmsg(LOG_ERR, "Could not unpack layer ID, %s at line %d column %d", error.text, error.line, error.column);
        }

        // Unpack type
        unpk = json_unpack_ex(layer, &error, 0, "{s:s}", "type", &ret[idx].type);

        if(unpk == -1){
            goto fail_layer;
        }

        // Unpack scalar values
        unpk = json_unpack_ex(layer,
                              &error,
                              0,
                              "{"
                              "s?b, s:b,"
                              "s?s, s:s, s?s,"
                              "s?i, s?i, s:i, s:i,"
                              "s?F, s?F, s:F, s?F, s?F"
                              "}",
                              "locked", &ret[idx].locked,
                              "visible", &ret[idx].visible,
                              "class", &ret[idx].class,
                              "name", &ret[idx].name,
                              "tintcolor", &ret[idx].tintcolor,
                              "startx", &ret[idx].startx,
                              "starty", &ret[idx].starty,
                              "x", &ret[idx].x,
                              "y", &ret[idx].y,
                              "offsetx", &ret[idx].offsetx,
                              "offsety", &ret[idx].offsety,
                              "opacity", &ret[idx].opacity,
                              "parallaxx", &ret[idx].parallaxx,
                              "parallaxy", &ret[idx].parallaxy
                             );

        if(unpk == -1){
            goto fail_layer;
        }

        // Unpack conditional scalar values
        if(strcmp(ret[idx].type, "imagelayer") == 0){
            unpk = json_unpack_ex(layer,
                                  &error,
                                  0,
                                  "{"
                                  "s:b, s:b,"
                                  "s:s, s?s"
                                  "}",
                                  "repeatx", &ret[idx].repeatx,
                                  "repeaty", &ret[idx].repeaty,
                                  "image", &ret[idx].image,
                                  "transparentcolor", &ret[idx].transparentcolor
                                 );

            if(unpk == -1){
                goto fail_layer;
            }
        }
        else if(strcmp(ret[idx].type, "tilelayer") == 0){
            unpk = json_unpack_ex(layer,
                                  &error,
                                  0,
                                  "{"
                                  "s?s, s?s,"
                                  "s:i, s:i,"
                                  "}",
                                  "compression", &ret[idx].compression,
                                  "encoding", &ret[idx].encoding,
                                  "height", &ret[idx].height,
                                  "width", &ret[idx].width
                                 );

            if(unpk == -1){
                goto fail_layer;
            }
        }
        else if(strcmp(ret[idx].type, "objectgroup") == 0){
            unpk = json_unpack_ex(layer,
                                  &error,
                                  0,
                                  "{s?s}",
                                  "draworder", &ret[idx].draworder
                                 );

            if(unpk == -1){
                goto fail_layer;
            }
        }

        // Unpack properties
        json_t* properties = NULL;

        unpk = json_unpack_ex(layer, &error, 0, "{s?o}", "properties", &properties);

        if(unpk == -1){
            goto fail_layer;
        }

        if(properties != NULL){
            ret[idx].properties = unpack_properties(properties);

            if(ret[idx].properties == NULL){
                logmsg(LOG_ERR, "Unable to unpack layer properties");

                goto fail_layer;
            }
        }

        // Unpack data
        if(strcmp(ret[idx].type, "tilelayer") == 0 ){
            json_t* data = NULL;

            unpk = json_unpack_ex(layer, &error, 0, "{s:o}", "data", &data);

            if(unpk == -1){
                goto fail_layer;
            }

            if(json_is_string(data)){
                ret[idx].data_is_str = true;

                unpk = json_unpack_ex(layer, &error, 0, "{s:s}", "data", &ret[idx].data_str);

                if(unpk == -1){
                    goto fail_layer;
                }
            }
            else if(json_is_array(data)){
                ret[idx].data_is_str = false;

                ret[idx].data_count = json_array_size(data);

                ret[idx].data_uint = calloc(ret[idx].data_count, sizeof(unsigned int));

                if(ret[idx].data_uint == NULL){
                    logmsg(LOG_ERR, "Unable to unpack data array, the system is out of memory");

                    goto fail_layer;
                }

                json_t* dat;

                size_t idx2;

                json_array_foreach(data, idx2, dat){
                    unpk = json_unpack_ex(dat, &error, 0, "i", &ret[idx].data_uint[idx2]);

                    if(unpk == -1){
                        goto fail_data;
                    }
                }
            }
            else{
                logmsg(LOG_ERR, "Layer data is neither a string nor an array");

                goto fail_layer;
            }
        }

        // Unpack chunks
        if(strcmp(ret[idx].type, "tilelayer") == 0 ){
            json_t* chunks = NULL;

            unpk = json_unpack_ex(layer, &error, 0, "{s?o}", "chunks", &chunks);

            if(unpk == -1){
                goto fail_data;
            }

            if(chunks != NULL){
                if(!json_is_array(chunks)){
                    logmsg(LOG_ERR, "Layer chunks must be an array");

                    goto fail_data;
                }

                ret[idx].chunk_count = json_array_size(chunks);

                ret[idx].chunks = calloc(ret[idx].chunk_count, sizeof(Chunk));

                if(ret[idx].chunks == NULL){
                    logmsg(LOG_ERR, "Unable to unpack layer chunks, the system is out of memory");

                    goto fail_data;
                }

                size_t idx2;
                json_t* chunk;

                json_array_foreach(chunks, idx2, chunk){
                    unpk = json_unpack_ex(chunk,
                                          &error,
                                          0,
                                          "{s:i, s:i, s:i, s:i}",
                                          "height", &ret[idx].chunks[idx2].height,
                                          "width", &ret[idx].chunks[idx2].width,
                                          "x", &ret[idx].chunks[idx2].x,
                                          "y", &ret[idx].chunks[idx2].y
                                         );

                    if(unpk == -1){
                        goto fail_chunk;
                    }

                    json_t* data = NULL;

                    unpk = json_unpack_ex(chunk, &error, 0, "{s:o}", "data", &data);

                    if(unpk == -1){
                        goto fail_chunk;
                    }

                    if(json_is_string(data)){
                        ret[idx].chunks[idx2].data_is_str = true;

                        unpk = json_unpack_ex(chunk, &error, 0, "{s:s}", "data", &ret[idx].chunks[idx2].data_str);

                        if(unpk == -1){
                            goto fail_chunk;
                        }
                    }
                    else if(json_is_array(data)){
                        ret[idx].chunks[idx2].data_is_str = false;

                        ret[idx].chunks[idx2].data_count = json_array_size(data);

                        ret[idx].chunks[idx2].data_uint = calloc(ret[idx].chunks[idx2].data_count, sizeof(unsigned int));

                        if(ret[idx].chunks[idx2].data_uint == NULL){
                            logmsg(LOG_ERR, "Unable to unpack chunk data array, the system is out of memory");

                            goto fail_chunk;
                        }

                        size_t idx3 = 0;
                        json_t* dat;

                        json_array_foreach(data, idx3, dat){
                            unpk = json_unpack_ex(dat, &error, 0, "i", &ret[idx].chunks[idx2].data_uint[idx3]);

                            if(unpk == -1){
                                goto fail_chunk_data;
                            }
                        }
                    }
                    else{
                        logmsg(LOG_ERR, "Chunk data is neither a string nor an array");

                        goto fail_chunk_data;
                    }
                }
            }
        }

        // Unpack objects
        if(strcmp(ret[idx].type, "objectgroup") == 0 ){
            
        }

        // Unpack nested layers (note: layers are optional, they don't necessarily exist
        if(strcmp(ret[idx].type, "group") == 0 ){
            json_t* nested_layers;

            unpk = json_unpack_ex(layer, &error, 0, "{s:o}", "layers", &nested_layers);

            if(unpk == -1){
                goto fail_object;
            }

            if(json_is_array(nested_layers) && json_array_size(nested_layers) > 0){
                ret[idx].layers = unpack_layers(nested_layers);

                if(ret[idx].layers == NULL){
                    logmsg(LOG_ERR, "Failed to load nested layers");

                    goto fail_object;
                }
            }
        }
    }
 
    return ret;

fail_object:

fail_chunk_data:
    for(size_t i = 0; i < layer_count; i++){
        for(size_t j = 0; i < ret[i].chunk_count; j++){
            if(!ret[i].chunks[j].data_is_str){
                free(ret[i].chunks[j].data_uint);
            }
        }
    }

fail_chunk:
    for(size_t i = 0; i < layer_count; i++){
        free(ret[i].chunks);
    }

fail_data:
    for(size_t i = 0; i < layer_count; i++){
        if(!ret[i].data_is_str){
            free(ret[i].data_uint);
        }
    }

fail_layer:
    logmsg(LOG_ERR, "Could not unpack layer %d, %s at line %d column %d", ret[idx].id, error.text, error.line, error.column);

    free(ret); 

    return NULL;
}


Map* map_load_json(const char* path){
    logmsg(LOG_DEBUG, "Loading JSON map file '%s'", path);

    json_error_t error;
    json_t* root = json_load_file(path, JSON_REJECT_DUPLICATES, &error);

    if(root == NULL){
        logmsg(LOG_ERR, "Could not load map '%s', %s at line %d, column %d", path, error.text, error.line, error.column);

        return NULL;
    }

    json_t* layers = NULL; 
    json_t* tilesets = NULL;
    json_t* properties = NULL;

    Map* map = calloc(1, sizeof(Map));
    if(map == NULL){
        logmsg(LOG_ERR, "Could not load map '%s', the system is out of memory", path);

        goto fail_map;
    }

    map->root = root;

    // Verify type (i.e, check that this is a map and not a tileset or something)
    int ret = json_unpack_ex(root, &error, 0, "{s:s}", "type", &map->type);

    if(ret == -1){
        goto fail_map;
    }

    if(strcmp(map->type, "map") != 0){
        logmsg(LOG_ERR, "File at path '%s' is of type '%s' and is not a map file", path, map->type);

        goto fail_map;
    }

    // Unpack scalar values
    ret = json_unpack_ex(root,
                         &error,
                         0,
                         "{"
                         "s:b,"
                         "s?s, s?s, s:s, s:s, s:s, s:s,"
                         "s:i, s:i, s:i, s:i, s:i, s:i, s:i,"
                         "s?F, s?F,"
                         "}",
                         "infinite", &map->infinite,
                         "backgroundcolor", &map->backgroundcolor,
                         "class", &map->class,
                         "orientation", &map->orientation,
                         "renderorder", &map->renderorder,
                         "tiledversion", &map->tiledversion,
                         "version", &map->version,
                         "compressionlevel", &map->compressionlevel,
                         "height", &map->height,
                         "nextlayerid", &map->nextlayerid,
                         "nextobjectid", &map->nextobjectid,
                         "tileheight", &map->tileheight,
                         "tilewidth", &map->tilewidth,
                         "width", &map->width,
                         "parallaxoriginx", &map->parallaxoriginx,
                         "parallaxoriginy", &map->parallaxoriginy
                        );

    if(ret == -1){
        goto fail_map;
    }

    // Unpack conditional scalar values
    if(strcmp(map->orientation, "staggered") == 0 || strcmp(map->orientation, "hexagonal") == 0){
        ret = json_unpack_ex(root,
                             &error,
                             0,
                             "{s:s, s:s}",
                             "staggeraxis", &map->staggeraxis,
                             "staggerindex", &map->staggerindex
                            );
        
        if(ret == -1){
            goto fail_map;
        }
    }

    if(strcmp(map->orientation, "hexagonal") == 0){
        ret = json_unpack_ex(root, &error, 0, "{s:i}", "hexsidelength", &map->hexsidelength);

        if(ret == -1){
            goto fail_map;
        }
    }

    // Unpack properties
    ret = json_unpack_ex(root, &error, 0, "{s?o}", "properties", &properties);

    if(ret == -1){
        goto fail_map;
    }

    if(properties != NULL){
        if(!json_is_array(properties)){
            logmsg(LOG_ERR, "Could not unpack map '%s', 'properties' must be an array", path);

            goto fail_map;
        }

        map->properties = unpack_properties(properties);

        if(map->properties == NULL){
            logmsg(LOG_ERR, "Unable to unpack map properties");

            goto fail_map;
        }
    }

    // Unpack layers
    ret = json_unpack_ex(root, &error, 0, "{s:o}", "layers", &layers);

    if(ret == -1){
        goto fail_map;
    }

    map->layers = unpack_layers(layers);

    if(map->layers == NULL){
        logmsg(LOG_ERR, "Could not unpack layers for map '%s'", path);

        goto fail_map;
    }

    // Unpack tilesets
    ret = json_unpack_ex(root, &error, 0, "{s:o}", "tilesets", &tilesets);

//    ret = json_unpack_ex(root, &error, 0, "{s:o}", "tilesets", &tilesets);
//
//    if(ret == -1){
//        goto fail_map;
//    }
//
//    map->tilesets = load_tilesets_json(tilesets);
//
//    if(map->tilesets == NULL){
//        logmsg(LOG_ERR, "Could not unpack tilesets for map '%s'", path);
//
//        goto fail_map;
//    }
//

    // Load tilesets
//    if(!json_is_array(tilesets)){
//        logmsg(LOG_ERR, "Could not unpack map '%s', 'tilesets' must be an array", path);
//
//        goto fail_tilesets;
//    }
//
//    size_t tileset_count = json_array_size(tilesets);
//
//    if(tileset_count < 1){
//        logmsg(LOG_ERR, "Unable to unpack map '%s', 'tilesets' array must have at least one element", path);
//
//        goto fail_tilesets;
//    }
//
//    map->tileset_count = tileset_count;
//
//    map->tilesets = calloc(tileset_count, sizeof(Tileset*));
//    if(map->tilesets == NULL){
//        logmsg(LOG_ERR, "Could not allocate space for tilesets, the system is out of memory");
//
//        goto fail_tilesets;
//    }
//
//    json_array_foreach(tilesets, index, value){
//        char* type = NULL;
//        char* source = NULL;
//
//        ret = json_unpack_ex(value, &error, 0, "{s:s}", "source", &source);
//
//        if(ret == -1){
//            logmsg(LOG_ERR, "Could not unpack tileset source for map '%s', %s", path, error.text);
//
//            goto fail_tilesets;
//        }
//
//        // The tileset path is relative to the map path, so we need to build it now
//        char* path_copy = strdup(path);
//
//        char* dir = dirname(path_copy);
//
//        char* tileset_path = calloc(1, strlen(dir) + strlen("/") + strlen(source));
//
//        char* to = tileset_path;
//
//        to = stpcpy(to, dir);
//        to = stpcpy(to, "/");
//        to = stpcpy(to, source);
//
//        free(path_copy);
//
//        map->tilesets[index] = tileset_load(tileset_path);
//        if(map->tilesets[index] == NULL){
//            logmsg(LOG_ERR, "Could not load tileset '%s'", tileset_path);
//
//            //Free any tilesets you've created so far, then free the tileset pointer array.
//
//            goto fail_tilesets;
//        }
//    }

    return map;


//fail_tilesets:
//    for(size_t i = 0; i < map->tileset_count; i++){
//        free(map->layers[i].data_uint);
//    }

fail_layers:
    free(map->layers);

    goto fail_map;

fail_map:
    logmsg(LOG_ERR, "Could not unpack map '%s', %s at line %d column %d", path, error.text, error.line, error.column);

    json_decref(root);

    free(map);

    return NULL;
}

//Tileset* tileset_load(const char* path){
//    json_error_t error;
//
//    json_t* root = json_load_file(path, JSON_REJECT_DUPLICATES, &error);
//    if(root == NULL){
//        logmsg(LOG_ERR, "Unable to load tileset '%s', %s at line %d, column %d", path, error.text, error.line, error.column);
//
//        return NULL;
//    }
//
//    Tileset* ts = calloc(1, sizeof(Tileset));
//    if(ts == NULL){
//        logmsg(LOG_ERR, "Unable to load tileset '%s', the system is out of memory", path);
//        
//        goto fail_tileset;
//    }
//
//    // Determine whether we need to load a sheet or a set of individual tile images
//    char* image_path = NULL;
//
//    int ret = json_unpack_ex(root, &error, 0, "s?s, s:F", "image", &image_path, "version", &ts->version);
//    if(ret == -1){
//        logmsg(LOG_ERR, "Unable to unpack tileset '%s', %s", path, error.text);
//
//        goto fail_tileset;
//    }
//
//    // If there's an "image" field in the root object, this is a sheet,
//    // otherwise there's a "tiles" array with individual tile objects in it.
//    if(image_path == NULL){
//        ts->type = INDIVIDUAL;
//
//        json_t* tiles = NULL;
//
//        ret = json_unpack_ex(root, &error, 0, "s:o", "tiles", &tiles);
//        if(ret == -1){
//            logmsg(LOG_ERR, "Unable to unpack tileset '%s', %s", path, error.text);
//
//            goto fail_tileset;
//        }
//
//        if(!json_is_array(tiles)){
//            logmsg(LOG_ERR, "Unable to unpack tileset '%s', 'tiles' must be an array", path);
//
//            goto fail_tileset;
//        }
//
//        size_t tile_count = json_array_size(tiles);
//
//        if(tile_count < 1){
//            logmsg(LOG_ERR, "Unable to unpack tileset '%s', 'tiles' array must have at least one element", path);
//
//            goto fail_tileset;
//        }
//
//        ts->tiles = calloc(tile_count, sizeof(Tile));
//        if(ts->tiles == NULL){
//            logmsg(LOG_ERR, "Unable to unpack tileset '%s', the system is out of memory", path);
//
//            goto fail_tileset;
//        }
//
//        size_t index;
//        json_t* value;
//        json_array_foreach(tiles, index, value){
//            ret = json_unpack_ex(   root,
//                                    &error,
//                                    0,
//                                    "{s:i, s:s, s:i, s:i}",
//                                    "id", &ts->tiles[index].id,
//                                    "image", &ts->tiles[index].image,
//                                    "imageheight", &ts->tiles[index].imageheight,
//                                    "imagewidth", &ts->tiles[index].imagewidth
//                                );
//
//            if(ret == -1){
//                logmsg(LOG_ERR, "Unable to unpack tileset '%s', %s", path, error.text);
//
//                free(ts->tiles);
//
//                goto fail_tileset;
//            }
//
//            //Either memory-map the file and save pointer in
//            //ts->tiles[index].data, or use SDL to load a surface directly, and
//            //save that in the tile object
//        }
//    }
//    else{
//        ts->type = SHEET;
//        
//        Sheet* sheet = calloc(1, sizeof(Sheet));
//        if(sheet == NULL){
//            logmsg(LOG_ERR, "Unable to load tileset '%s', the system is out of memory", path);
//
//            goto fail_tileset;
//        }
//    }
//
//fail_tileset:
//    json_decref(root);
//
//    free(ts);
//
//    return NULL;
//}

Map* map_load(const char* path){
    char* ext = strrchr(path, '.');

    // No extension
    if(ext == NULL){
        logmsg(LOG_ERR, "Map filename has no extension");
        return NULL;
    }

    // JSON map
    if(strcmp(ext, ".tmj") == 0 || strcmp(ext, ".json") == 0){
        return map_load_json(path);
    }
    // Unknown extension
    else{
        logmsg(LOG_ERR, "Map filename has unknown extension, '%s'", ext);
        return NULL;
    }
}
