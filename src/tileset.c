#include <string.h>

#include <jansson.h>

#include "log.h"
#include "map.h"
#include "tmj.h"

/**
 * @file
 */

int unpack_tileset(json_t* tileset, Tileset* ret){
    logmsg(DEBUG, "Unpacking tileset");

    if(tileset == NULL){
        return -1;
    }

    json_error_t error;

    json_t* grid = NULL;
    json_t* tileoffset = NULL;
    json_t* transformations = NULL;
    json_t* properties = NULL;
    json_t* terrains = NULL;
    json_t* tiles = NULL;

    // Unpack scalar values
    int unpk = json_unpack_ex(tileset,
                              &error,
                              0,
                              "{"
                              "s?s, s?s, s?s, s:s, s:s, s?s, s?s, s:s, s?s, s?s, s:s, s:s,"
                              "s:i, s?i, s:i, s:i, s:i, s:i, s:i, s:i, s:i,"
                              "s?o, s?o, s?o, s?o, s?o, s?o"
                              "}",
                              "backgroundcolor", &ret->backgroundcolor,
                              "class", &ret->class,
                              "fillmode", &ret->fillmode,
                              "image", &ret->image,
                              "name", &ret->name,
                              "objectalignment", &ret->objectalignment,
                              "source", &ret->source,
                              "tiledversion", &ret->tiledversion,
                              "tilerendersize", &ret->tilerendersize,
                              "transparentcolor", &ret->transparentcolor,
                              "type", &ret->type,
                              "version", &ret->version,
                              "columns", &ret->columns,
                              "firstgid", &ret->firstgid,
                              "imageheight", &ret->imageheight,
                              "imagewidth", &ret->imagewidth,
                              "margin", &ret->margin,
                              "spacing", &ret->spacing,
                              "tilecount", &ret->tilecount,
                              "tileheight", &ret->tileheight,
                              "tilewidth", &ret->tilewidth,
                              "grid", &grid,
                              "tileoffset", &tileoffset,
                              "transformations", &transformations,
                              "properties", &properties,
                              "terrains", &terrains,
                              "tiles", &tiles
                             );

    if(unpk == -1){
        logmsg(ERR, "Unable to unpack tileset, %s at line %d column %d", error.text, error.line, error.column);

        return -1;
    }

    // Unpack Grid
    if(grid){
        ret->grid = calloc(1, sizeof(Grid));

        if(ret->grid == NULL){
            logmsg(ERR, "Unable to unpack tileset[%s]->grid, the system is out of memory", ret->name);

            return -1;
        }

        unpk = json_unpack_ex(grid,
                              &error,
                              0,
                              "{s:i, s:s, s:i}",
                              "height", &ret->grid->height,
                              "orientation", &ret->grid->orientation,
                              "width", &ret->grid->width
                             );

        if(unpk == -1){
            logmsg(ERR, "Unable to unpack tileset[%s]->grid, %s at line %d column %d", ret->name, error.text, error.line, error.column);

            goto fail_grid;
        }
    }

    // Unpack TileOffset
    if(tileoffset){
        ret->tileoffset = calloc(1, sizeof(TileOffset));

        if(ret->tileoffset == NULL){
            logmsg(ERR, "Unable to unpack tileset[%s]->tileoffset, the system is out of memory", ret->name);

            goto fail_grid;
        }

        unpk = json_unpack_ex(tileoffset,
                              &error,
                              0,
                              "{s:i, s:i}",
                              "x", &ret->tileoffset->x,
                              "y", &ret->tileoffset->y
                             );

        if(unpk == -1){
            logmsg(ERR, "Unable to unpack tileset[%s]->tileoffset, %s at line %d column %d", ret->name, error.text, error.line, error.column);

            goto fail_tileoffset;
        }
    }

    // Unpack Transformations
    if(transformations){
        ret->transformations = calloc(1, sizeof(Transformations));

        if(ret->transformations == NULL){
            logmsg(ERR, "Unable to unpack tileset[%s]->transformations, the system is out of memory", ret->name);

            goto fail_tileoffset;
        }

        unpk = json_unpack_ex(transformations,
                              &error,
                              0,
                              "{s:b, s:b, s:b, s:b}",
                              "hflip", &ret->transformations->hflip,
                              "vfilp", &ret->transformations->vflip,
                              "rotate", &ret->transformations->rotate,
                              "preferuntransformed", &ret->transformations->preferuntransformed
                             );

        if(unpk == -1){
            logmsg(ERR, "Unable to unpack tileset[%s]->transformations, %s at line %d column %d", ret->name, error.text, error.line, error.column);

            goto fail_transformations;
        }
    }

    // Unpack Properties
    if(properties){
        if((ret->properties = unpack_properties(properties)) == NULL){
            logmsg(ERR, "Unable to unpack tileset[%s]->properties", ret->name);

            goto fail_transformations;
        }

        ret->property_count = json_array_size(properties);
    }

    // Unpack Terrains
    if(terrains){
        if(!json_is_array(terrains)){
            logmsg(ERR, "Unable to unpack tileset[%s]->terrains, terrains must be an array of Terrains", ret->name);

            goto fail_properties;
        }

        ret->terrain_count = json_array_size(terrains);

        ret->terrains = calloc(ret->terrain_count, sizeof(Terrain));

        if(ret->terrains == NULL){
            logmsg(ERR, "Unable to unpack tileset[%s]->terrains, the system is out of memory", ret->name);

            goto fail_properties;
        }

        size_t idx;
        json_t* terrain;

        json_array_foreach(terrains, idx, terrain){
            properties = NULL;

            unpk = json_unpack_ex(terrain,
                                  &error,
                                  0,
                                  "{s:s, s:i, s?o}",
                                  "name", &ret->terrains[idx].name,
                                  "tile", &ret->terrains[idx].tile,
                                  "properties", &properties
                                 );

            if(unpk == -1){
                logmsg(ERR, "Unable to unpack tileset[%s]->terrains, %s at line %d column %d", ret->name, error.text, error.line, error.column);

                goto fail_terrains;
            }

            if(properties){
                if((ret->terrains[idx].properties = unpack_properties(properties)) == NULL){
                    logmsg(ERR, "Unable to unpack tileset[%s]->terrain[%s]->properties", ret->name, ret->terrains[idx].name);

                    goto fail_terrains;
                }

                ret->terrains[idx].property_count = json_array_size(properties);
            }
        }
    }

    // Unpack Tiles
    if(tiles){
        if(!json_is_array(tiles)){
            logmsg(ERR, "Unable to unpack tileset[%s]->tiles, tiles must be an array of Tiles", ret->name);

            goto fail_terrains;
        }

        ret->tile_count = json_array_size(tiles);

        ret->tiles = calloc(ret->tile_count, sizeof(Tile));

        if(ret->tiles == NULL){
            logmsg(ERR, "Unable to unpack tileset[%s]->tiles, the system is out of memory", ret->name);

            goto fail_tiles;
        }

        size_t idx = 0;
        json_t* tile = NULL;

        json_array_foreach(tiles, idx, tile){
            // Unpack Tile scalar values
            json_t* animation = NULL;
            json_t* objectgroup = NULL;
            json_t* properties = NULL;
            json_t* terrain = NULL;

            unpk = json_unpack_ex(tile,
                                  &error,
                                  0,
                                  "{"
                                  "s?s, s?s,"
                                  "s:i, s?i, s?i, s?i, s?i, s?i, s?i,"
                                  "s?F,"
                                  "s?o, s?o, s?o, s?o"
                                  "}",
                                  "class", &ret->tiles[idx].class,
                                  "image", &ret->tiles[idx].image,
                                  "id", &ret->tiles[idx].id,
                                  "imageheight", &ret->tiles[idx].imageheight,
                                  "imagewidth", &ret->tiles[idx].imagewidth,
                                  "x", &ret->tiles[idx].x,
                                  "y", &ret->tiles[idx].y,
                                  "width", &ret->tiles[idx].width,
                                  "height", &ret->tiles[idx].height,
                                  "probability", &ret->tiles[idx].probability,
                                  "animation", &animation,
                                  "objectgroup", &objectgroup,
                                  "properties", &properties,
                                  "terrain", &terrain
                                 );

            if(unpk == -1){
                logmsg(ERR, "Unable to unpack tileset[%s]->tiles, %s at line %d column %d", ret->name, error.text, error.line, error.column);

                goto fail_tiles;
            }

            // Unpack Tile objectgroup
            if(objectgroup){
                ret->tiles[idx].objectgroup = calloc(1, sizeof(Layer));

                if(ret->tiles[idx].objectgroup == NULL){
                    logmsg(ERR, "Unable to unpack tileset[%s]->tiles[%d]->objectgroup, the system is out of memory", ret->name, ret->tiles[idx].id);

                    goto fail_tiles;
                }

                json_t* objects = NULL;
                json_t* layer_properties = NULL;

                unpk = json_unpack_ex(objectgroup,
                                      &error,
                                      0,
                                      "{"
                                      "s?b, s:b,"
                                      "s?s, s?s, s:s, s?s, s:s,"
                                      "s?i, s?i, s?i,"
                                      "s?F, s?F, s:F, s?F, s?F,"
                                      "s?o, s?o"
                                      "}",
                                      "locked", &ret->tiles[idx].objectgroup->locked,
                                      "visible", &ret->tiles[idx].objectgroup->visible,
                                      "class", &ret->tiles[idx].objectgroup->class,
                                      "draworder", &ret->tiles[idx].objectgroup->draworder,
                                      "name", &ret->tiles[idx].objectgroup->name,
                                      "tintcolor", &ret->tiles[idx].objectgroup->tintcolor,
                                      "type", &ret->tiles[idx].objectgroup->type,
                                      "id", &ret->tiles[idx].objectgroup->id,
                                      "x", &ret->tiles[idx].objectgroup->x,
                                      "y", &ret->tiles[idx].objectgroup->y,
                                      "offsetx", &ret->tiles[idx].objectgroup->offsetx,
                                      "offsety", &ret->tiles[idx].objectgroup->offsety,
                                      "opacity", &ret->tiles[idx].objectgroup->opacity,
                                      "parallaxx", &ret->tiles[idx].objectgroup->parallaxx,
                                      "parallaxy", &ret->tiles[idx].objectgroup->parallaxy,
                                      "objects", &objects,
                                      "properties", &layer_properties
                                      );

                if(unpk == -1){
                    logmsg(ERR, "Unable to unpack tileset[%s]->tiles[%d]->objectgroup, %s at line %d column %d", ret->name, ret->tiles[idx].id, error.text, error.line, error.column);

                    goto fail_tiles;
                }

                if(objects){
                    ret->tiles[idx].objectgroup->objects = unpack_objects(objects);

                    if(ret->tiles[idx].objectgroup->objects == NULL){
                        logmsg(ERR, "Unable to unpack tileset[%s]->tiles[%d]->objectgroup->objects", ret->name, ret->tiles[idx].id);

                        goto fail_tiles;
                    }

                    ret->tiles[idx].objectgroup->object_count = json_array_size(objects);
                }

                if(layer_properties){
                    ret->tiles[idx].objectgroup->properties = unpack_properties(layer_properties);

                    if(ret->tiles[idx].objectgroup->properties == NULL){
                        logmsg(ERR, "Unable to unpack tileset[%s]->tiles[%d]->objectgroup->properties", ret->name, ret->tiles[idx].id);

                        goto fail_tiles;
                    }

                    ret->tiles[idx].objectgroup->property_count = json_array_size(layer_properties);
                }
            }

            // Unpack Tile animation
            if(animation){
                if(!json_is_array(animation)){
                    logmsg(ERR, "Unable to unpack tileset[%s]->tiles[%d]->animation, animation must be an array of Frames", ret->name, ret->tiles[idx].id);

                    goto fail_tiles;
                }

                ret->tiles[idx].animation = calloc(json_array_size(animation), sizeof(Frame));

                if(ret->tiles[idx].animation == NULL){
                    logmsg(ERR, "Unable to unpack tileset[%s]->tiles[%d]->animation, the system is out of memory", ret->name, ret->tiles[idx].id);

                    goto fail_tiles;
                }

                size_t idx2 = 0;
                json_t* frame = NULL;

                json_array_foreach(animation, idx2, frame){
                    unpk = json_unpack_ex(frame,
                                          &error,
                                          0,
                                          "{"
                                          "s:i, s:i"
                                          "}",
                                          "duration", &ret->tiles[idx].animation[idx2].duration,
                                          "tileid", &ret->tiles[idx].animation[idx2].tileid
                                         );

                    if(unpk == -1){
                        logmsg(ERR, "Unable to unpack tileset[%s]->tiles[%d]->animation, %s at line %d column %d", ret->name, ret->tiles[idx].id, error.text, error.line, error.column);

                        goto fail_tiles;
                    }
                }

                ret->tiles[idx].animation_count = json_array_size(animation);
            }

            // Unpack Tile properties
            if(properties){
                ret->tiles[idx].properties = unpack_properties(properties);

                if(ret->tiles[idx].properties == NULL){
                    logmsg(ERR, "Unable to unpack tileset[%s]->tiles[%d]->properties", ret->name, ret->tiles[idx].id);

                    goto fail_tiles;
                }

                ret->tiles[idx].property_count = json_array_size(properties);
            }

            // Unpack Tile terrain
            if(terrain){
                if(!json_is_array(terrain)){
                    logmsg(ERR, "Unable to unpack tileset[%s]->tiles[%d]->terrain, terrain must be an array of terrain indexes", ret->name, ret->tiles[idx].id);

                    goto fail_tiles;
                }

                size_t idx2 = 0;
                json_t* terrain_idx = NULL;

                json_array_foreach(terrain, idx2, terrain_idx){
                    if(!json_is_integer(terrain_idx)){
                        logmsg(ERR, "Unable to unpack tileset[%s]->tiles[%d]->terrain, terrain must be an array of integers");

                        goto fail_tiles;
                    }

                    ret->tiles[idx].terrain[idx2] = json_integer_value(terrain_idx);
                }
            }
        }
    }

    return 0;

fail_tiles:
    for(size_t i = 0; i < ret->tile_count; i++){
        free(ret->tiles[i].animation);
        if(ret->tiles[i].objectgroup != NULL){
            free(ret->tiles[i].objectgroup->properties);
            free_objects(ret->tiles[i].objectgroup->objects, ret->tiles[i].objectgroup->object_count);
        }
        free(ret->tiles[i].objectgroup);
        free(ret->tiles[i].properties);
    }

    free(ret->tiles);

fail_terrains:
    for(size_t i = 0; i < ret->terrain_count; i++){
        free(ret->terrains[i].properties);
    }

    free(ret->terrains);

fail_properties:
    free(ret->properties);

fail_transformations:
    free(ret->transformations);

fail_tileoffset:
    free(ret->tileoffset);

fail_grid:
    free(ret->grid);

    return -1;
}

/**
 * Helper function for freeing tilesets embedded in maps
 */
void tilesets_free(Tileset* tilesets, size_t tileset_count){
    for(size_t i = 0; i < tileset_count; i++){
        // Free tiles
        if(tilesets[i].tiles){
            for(size_t j = 0; j < tilesets[i].tile_count; j++){
                free(tilesets[i].tiles->animation);
                if(tilesets[i].tiles->objectgroup != NULL){
                    free(tilesets[i].tiles->objectgroup->properties);
                    free_objects(tilesets[i].tiles->objectgroup->objects, tilesets[i].tiles->objectgroup->object_count);
                }
                free(tilesets[i].tiles->objectgroup);
                free(tilesets[i].tiles->properties);
            }

            free(tilesets[i].tiles);
        }

        // Free terrains
        if(tilesets[i].terrains){
            for(size_t j = 0; j < tilesets[i].terrain_count; j++){
                free(tilesets[i].terrains[j].properties);
            }

            free(tilesets[i].terrains);
        }

        // Free everything else
        free(tilesets[i].properties);
        free(tilesets[i].transformations);
        free(tilesets[i].tileoffset);
        free(tilesets[i].grid);
    }

    free(tilesets);
}

Tileset* tmj_tileset_loadf(const char* path, bool check_extension){
    logmsg(DEBUG, "Loading JSON tileset file '%s'", path);

    char* ext = strrchr(path, '.');

    if(check_extension){
        if(ext == NULL){
            logmsg(ERR, "Tileset filename '%s' has no extension", path);

            return NULL;
        }

        if(strcmp(ext, ".tsj") != 0 && strcmp(ext, ".json") != 0){
            logmsg(ERR, "Tileset filename '%s' has unknown extension, '%s'", path, ext);
            logmsg(ERR, "Tileset filename '%s' must have '.tsj' or '.json' extension to be loaded", path);

            return NULL;
        }
    }

    json_error_t error;
    json_t* root = json_load_file(path, JSON_REJECT_DUPLICATES, &error);

    if(root == NULL){
        logmsg(ERR, "Could not load tileset '%s', %s at line %d column %d", path, error.text, error.line, error.column);

        return NULL;
    }

    Tileset* ret = calloc(1, sizeof(Tileset));

    if(ret == NULL){
        logmsg(ERR, "Unable to load tileset[%s], the system is out of memory", path);

        return NULL;
    }

    if(unpack_tileset(root, ret) == -1){
        logmsg(ERR, "Unable to unpack tileset[%s]", path);

        free(ret);

        return NULL;
    }

    ret->root = root;

    return ret;
}

Tileset* tmj_tileset_load(const char* tileset){
    logmsg(DEBUG, "Loading JSON tileset from string");

    json_error_t error;
    json_t* root = json_loads(tileset, JSON_REJECT_DUPLICATES, &error);

    if(root == NULL){
        logmsg(ERR, "Could not load tileset, %s at line %d column %d", error.text, error.line, error.column);

        return NULL;
    }

    Tileset* ret = calloc(1, sizeof(Tileset));

    if(ret == NULL){
        logmsg(ERR, "Unable to load tileset, the system is out of memory");

        return NULL;
    }

    if(unpack_tileset(root, ret) == -1){
        logmsg(ERR, "Unable to unpack tileset");

        free(ret);

        return NULL;
    }

    ret->root = root;

    return ret;
}

void tmj_tileset_free(Tileset* tileset){
    tilesets_free(tileset, 1);
}
