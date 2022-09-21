#include <libgen.h>
#include <string.h>
#include <unistd.h>

#include <jansson.h>

#include "log.h"
#include "tmj.h"

/**
 * @file
 */

Property* unpack_properties(json_t* properties){
    if(properties == NULL){
        return NULL;
    }

    if(!json_is_array(properties)){
        logmsg(ERR, "'properties' must be an array");

        return NULL;
    }

    json_error_t error;

    size_t property_count = json_array_size(properties);

    Property* ret = calloc(property_count, sizeof(Property));

    if(ret == NULL){
        logmsg(ERR, "Unable to unpack properties, the system is out of memory");

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
            logmsg(ERR, "Unable to unpack properties, %s at line %d column %d", error.text, error.line, error.column);

            free(ret);

            return NULL;
        }
    }

    return ret;
}


int unpack_tileset(json_t* tileset, Tileset** ret){
    return 0;
}

/**
 * Helper function for freeing tilesets embedded in maps
 */
void free_tilesets(Tileset* tilesets, size_t tileset_count){
    
}

Tileset* tileset_load_file(const char* path, bool check_extension){
    logmsg(DEBUG, "Loading JSON tileset file '%s'", path);

    json_error_t error;
    json_t* tileset = json_load_file(path, JSON_REJECT_DUPLICATES, &error);

    if(tileset == NULL){
        logmsg(ERR, "Could not load tileset '%s', %s at line %d, column %d", path, error.text, error.line, error.column);

        return NULL;
    }

    Tileset* ret = calloc(1, sizeof(Tileset));

    if(ret == NULL){
        logmsg(ERR, "Tileset '%s': Unable to load tileset, the system is out of memory", path);

        return NULL;
    }

    if(unpack_tileset(tileset, &ret) == -1){
        logmsg(ERR, "Tileset '%s': Unable to unpack tileset", path);

        free(ret);

        return NULL;
    }

    return ret;
}

Tileset* tileset_load(const char* tileset){
    return NULL;
}

/**
 * Unpacks an array of points. The returned array must be freed by the caller.
 */
Point* unpack_points(json_t* points){
    if(points == NULL){
        return NULL;
    }

    if(!json_is_array(points)){
        logmsg(ERR, "'points' must be an array");

        return NULL;
    }

    json_error_t error;

    size_t point_count = json_array_size(points);

    Point* ret = calloc(point_count, sizeof(Point));

    if(ret == NULL){
        logmsg(ERR, "Unable to unpack points, the system is out of memory");

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
            logmsg(ERR, "Unable to unpack points, %s at line %d column %d", error.text, error.line, error.column);

            free(ret);

            return NULL;
        }
    }

    return ret;
}

/**
 * Unpacks a text object. The returned object must be freed by the caller.
 */
Text* unpack_text(json_t* text){
    if(text == NULL){
        return NULL;
    }

    json_error_t error;

    Text* ret = calloc(1, sizeof(Text));

    if(ret == NULL){
        logmsg(ERR, "Unable to unpack text, the system is out of memory");

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
        logmsg(ERR, "Unable to unpack text, %s at line %d column %d", error.text, error.line, error.column);

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
        logmsg(ERR, "'objects' must be an array");
    }

    json_error_t error;

    size_t object_count = json_array_size(objects);

    Object* ret = calloc(object_count, sizeof(Object));

    if(ret == NULL){
        logmsg(ERR, "Unable to unpack objects, the system is out of memory");

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
            logmsg(ERR, "Unable to unpack object, %s at line %d column %d", error.text, error.line, error.column);

            return NULL;
        }

        //Unpack properties
        json_t* properties = NULL;

        unpk = json_unpack_ex(object, &error, 0, "{s?o}", "properties", &properties);

        if(unpk == -1){
            logmsg(ERR, "Unable to unpack object, %s at line %d column %d", error.text, error.line, error.column);

            goto fail_properties;
        }

        if(properties != NULL){
            ret[idx].properties = unpack_properties(properties);

            if(ret[idx].properties == NULL){
                logmsg(ERR, "Unable to unpack object properties");

                goto fail_properties;
            }

            ret[idx].property_count = json_array_size(properties);
        }

        // Unpack text
        json_t* text = NULL;

        unpk = json_unpack_ex(object, &error, 0, "{s?o}", "text", &text);

        if(unpk == -1){
            logmsg(ERR, "Unable to unpack object, %s at line %d column %d", error.text, error.line, error.column);

            goto fail_text;
        }

        if(text != NULL){
            ret[idx].text = unpack_text(text);

            if(ret[idx].text == NULL){
                logmsg(ERR, "Unable to unpack object text");

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
            logmsg(ERR, "Unable to unpack object, %s at line %d column %d", error.text, error.line, error.column);

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
            logmsg(ERR, "Unable to unpack object, %s at line %d column %d", error.text, error.line, error.column);

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
 * Helper function to free Objects. May cause undefined behavior if the objects
 * were modified by the caller of map_load().
 */
void free_objects(Object* objects, size_t object_count){
    for(size_t i = 0; i < object_count; i++){
        // We don't bother freeing polyline, because polygon and polyline are a union
        free(objects[i].polygon);
        free(objects[i].text);
        free(objects[i].properties);
    }

    free(objects);
}

Chunk* unpack_chunks(json_t* chunks){
    if(chunks == NULL){
        return NULL;
    }

    if(!json_is_array(chunks)){
        logmsg(ERR, "Could not unpack layer chunks, 'chunks' must be an array");

        return NULL;
    }

    json_error_t error;

    size_t chunk_count = json_array_size(chunks);

    Chunk* ret = calloc(chunk_count, sizeof(Chunk));

    if(ret == NULL){
        logmsg(ERR, "Unable to unpack chunks, the system is out of memory");

        return NULL;
    }

    size_t idx;
    json_t* chunk;

    json_array_foreach(chunks, idx, chunk){
        json_t* data = NULL;

        int unpk = json_unpack_ex(chunk,
                                  &error,
                                  0,
                                  "{"
                                  "s:i, s:i, s:i, s:i,"
                                  "s:o,"
                                  "}",
                                  "height", &ret[idx].height,
                                  "width", &ret[idx].width,
                                  "x", &ret[idx].x,
                                  "y", &ret[idx].y,
                                  "data", &data
                                 );

        if(unpk == -1){
            logmsg(ERR, "Unable to unpack chunk, %s at line %d column %d", error.text, error.line, error.column);

            goto fail_chunk;
        }

        if(json_is_string(data)){
            unpk = json_unpack_ex(data, &error, 0, "s", &ret[idx].data_str);

            if(unpk == -1){
                logmsg(ERR, "Unable to unpack chunk data, %s at line %d column %d", error.text, error.line, error.column);

                goto fail_chunk;
            }

            ret[idx].data_is_str = true;
        }
        else if(json_is_array(data)){
            size_t datum_count = json_array_size(data);

            ret[idx].data_uint = calloc(datum_count, sizeof(unsigned int));

            if(ret[idx].data_uint == NULL){
                logmsg(ERR, "Unable to unpack chunk data, the system is out memory");

                goto fail_chunk;
            }

            size_t idx2;
            json_t* datum;

            json_array_foreach(data, idx2, datum){
                unpk = json_unpack_ex(datum, &error, 0, "i", &ret[idx].data_uint[idx2]);

                if(unpk == -1){
                    logmsg(ERR, "Unable to unpack chunk datum, %s at line %d column %d", error.text, error.line, error.column);

                    goto fail_data;
                }
            }
        }
        else{
            logmsg(ERR, "Unable to unpack chunk, chunk data must be a string or an array of uint");

            goto fail_chunk;
        }
    }

    return ret;

fail_data:
    for(size_t i = 0; i < chunk_count; i++){
        if(!ret[i].data_is_str){
            free(ret[i].data_uint);
        }
    }

fail_chunk:
    free(ret);

    return NULL;
}

// Helper function for freeing chunks, since they contain dynamically-allocated arrays
void free_chunks(Chunk* chunks, size_t chunk_count){
    for(size_t i = 0; i < chunk_count; i++){
        if(!chunks[i].data_is_str){
            free(chunks[i].data_uint);
        }
    }

    free(chunks);
}

/**
 * Loads map layers recursively
 */
Layer* unpack_layers(json_t* layers){
    if(!json_is_array(layers)){
        logmsg(ERR, "Could not unpack layer, 'layers' must be an array");

        return NULL;
    }

    size_t layer_count = json_array_size(layers);

    if(layer_count < 1){
        logmsg(ERR, "Unable to load layers, 'layers' array must have at least one element");

        return NULL;
    }

    Layer* ret = calloc(layer_count, sizeof(Layer));

    if(ret == NULL){
        logmsg(ERR, "Unable to load layers, the system is out of memory");

        return NULL;
    }

    size_t idx;
    json_t* layer;
    json_error_t error;

    json_array_foreach(layers, idx, layer){
        // Unpack id so we can log errors with it
        int unpk = json_unpack_ex(layer, &error, 0, "{s:i}", "id", &ret[idx].id);

        if(unpk == -1){
            logmsg(ERR, "Could not unpack layer ID, %s at line %d column %d", error.text, error.line, error.column);
        }

        logmsg(DEBUG, "Loading layer %d", ret[idx].id);

        // Unpack type
        unpk = json_unpack_ex(layer, &error, 0, "{s:s}", "type", &ret[idx].type);

        if(unpk == -1){
            logmsg(ERR, "Could not unpack layer %d, %s at line %d column %d", ret[idx].id, error.text, error.line, error.column);

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
            logmsg(ERR, "Could not unpack layer %d, %s at line %d column %d", ret[idx].id, error.text, error.line, error.column);

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
                logmsg(ERR, "Could not unpack layer %d, %s at line %d column %d", ret[idx].id, error.text, error.line, error.column);

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
                logmsg(ERR, "Could not unpack layer %d, %s at line %d column %d", ret[idx].id, error.text, error.line, error.column);

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
                logmsg(ERR, "Could not unpack layer %d, %s at line %d column %d", ret[idx].id, error.text, error.line, error.column);

                goto fail_layer;
            }
        }

        // Unpack data
        if(strcmp(ret[idx].type, "tilelayer") == 0 ){
            json_t* data = NULL;

            unpk = json_unpack_ex(layer, &error, 0, "{s:o}", "data", &data);

            if(unpk == -1){
                logmsg(ERR, "Could not unpack layer %d, %s at line %d column %d", ret[idx].id, error.text, error.line, error.column);

                goto fail_layer;
            }

            if(json_is_string(data)){
                ret[idx].data_is_str = true;

                unpk = json_unpack_ex(layer, &error, 0, "{s:s}", "data", &ret[idx].data_str);

                if(unpk == -1){
                    logmsg(ERR, "Could not unpack layer %d, %s at line %d column %d", ret[idx].id, error.text, error.line, error.column);

                    goto fail_layer;
                }
            }
            else if(json_is_array(data)){
                ret[idx].data_is_str = false;

                ret[idx].data_count = json_array_size(data);

                ret[idx].data_uint = calloc(ret[idx].data_count, sizeof(unsigned int));

                if(ret[idx].data_uint == NULL){
                    logmsg(ERR, "Layer %d: Unable to unpack data array, the system is out of memory", ret[idx].id);

                    goto fail_layer;
                }

                json_t* datum;

                size_t idx2;

                json_array_foreach(data, idx2, datum){
                    unpk = json_unpack_ex(datum, &error, 0, "i", &ret[idx].data_uint[idx2]);

                    if(unpk == -1){
                        goto fail_data;
                    }
                }
            }
            else{
                logmsg(ERR, "Layer %d: Layer data is neither a string nor an array", ret[idx].id);

                goto fail_layer;
            }
        }

        // Unpack properties
        json_t* properties = NULL;

        unpk = json_unpack_ex(layer, &error, 0, "{s?o}", "properties", &properties);

        if(unpk == -1){
            goto fail_data;
        }

        if(properties != NULL){
            ret[idx].properties = unpack_properties(properties);

            if(ret[idx].properties == NULL){
                logmsg(ERR, "Layer %d: Unable to unpack layer properties", ret[idx].id);

                goto fail_data;
            }
        }

        // Unpack chunks
        if(strcmp(ret[idx].type, "tilelayer") == 0 ){
            json_t* chunks = NULL;

            unpk = json_unpack_ex(layer, &error, 0, "{s?o}", "chunks", &chunks);

            if(unpk == -1){
                goto fail_properties;
            }

            if(chunks != NULL){
                ret[idx].chunks = unpack_chunks(chunks);

                if(ret[idx].chunks == NULL){
                    logmsg(ERR, "Layer %d: Unable to unpack layer chunks", ret[idx].id);

                    goto fail_properties;
                }
            }
        }

        // Unpack objects
        if(strcmp(ret[idx].type, "objectgroup") == 0 ){
            json_t* objects = NULL;

            unpk = json_unpack_ex(layer, &error, 0, "{s:o}", "objects", &objects);

            if(unpk == -1){
                goto fail_chunks;
            }

            if(objects != NULL){
                ret[idx].objects = unpack_objects(objects);

                if(ret[idx].objects == NULL){
                    logmsg(ERR, "Layer %d: Unable to unpack layer objects", ret[idx].id);

                    goto fail_chunks;
                }
            }
        }

        // Unpack nested layers (note: layers are optional, they don't necessarily exist
        if(strcmp(ret[idx].type, "group") == 0 ){
            json_t* nested_layers = NULL;

            unpk = json_unpack_ex(layer, &error, 0, "{s:o}", "layers", &nested_layers);

            if(unpk == -1){
                goto fail_objects;
            }

            if(json_is_array(nested_layers) && json_array_size(nested_layers) > 0){
                ret[idx].layers = unpack_layers(nested_layers);

                if(ret[idx].layers == NULL){
                    logmsg(ERR, "Layer %d: Failed to load nested layers", ret[idx].id);

                    goto fail_objects;
                }
            }
        }
    }
 
    return ret;

fail_objects:
    for(size_t i = 0; i < layer_count; i++){
        free_objects(ret[i].objects, ret[i].object_count);
    }
fail_chunks:
    for(size_t i = 0; i < layer_count; i++){
        free_chunks(ret[i].chunks, ret[i].chunk_count);
    }

fail_properties:
    for(size_t i = 0; i < layer_count; i++){
        free(ret[i].properties);
    }

fail_data:
    for(size_t i = 0; i < layer_count; i++){
        if(!ret[i].data_is_str){
            free(ret[i].data_uint);
        }
    }

fail_layer:
    free(ret); 

    return NULL;
}

/**
 * Helper function for freeing layer tree associated with a map. May result in
 * undefined behavior if the layer objects were modified by the caller of
 * map_load().
 */
void free_layers(Layer* layers, size_t layer_count){
    for(size_t i = 0; i < layer_count; i++){
        free_objects(layers[i].objects, layers[i].object_count);
        free_chunks(layers[i].chunks, layers[i].chunk_count);
        free(layers[i].properties);
        free(layers[i].data_uint);

        free_layers(layers[i].layers, layers[i].layer_count);
    }

    free(layers);
}

Map* map_load_json_file(const char* path){
    logmsg(DEBUG, "Loading JSON map file '%s'", path);

    json_error_t error;
    json_t* root = json_load_file(path, JSON_REJECT_DUPLICATES, &error);

    if(root == NULL){
        logmsg(ERR, "Could not load map '%s', %s at line %d, column %d", path, error.text, error.line, error.column);

        return NULL;
    }

    json_t* layers = NULL; 
    json_t* tilesets = NULL;
    json_t* properties = NULL;

    Map* map = calloc(1, sizeof(Map));

    if(map == NULL){
        logmsg(ERR, "Could not load map '%s', the system is out of memory", path);

        goto fail_map;
    }

    map->root = root;

    // Verify type (i.e, check that this is a map and not a tileset or something)
    int unpk = json_unpack_ex(root, &error, 0, "{s:s}", "type", &map->type);

    if(unpk == -1){
        logmsg(ERR, "Map '%s': Could not unpack map type, %s at line %d, column %d", path, error.text, error.line, error.column);

        goto fail_map;
    }

    if(strcmp(map->type, "map") != 0){
        logmsg(ERR, "File at path '%s' is of type '%s' and is not a map file", path, map->type);

        goto fail_map;
    }

    // Unpack scalar values
    unpk = json_unpack_ex(root,
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

    if(unpk == -1){
        logmsg(ERR, "Map '%s': Could not unpack map, %s at line %d, column %d", path, error.text, error.line, error.column);

        goto fail_map;
    }

    // Unpack conditional scalar values
    if(strcmp(map->orientation, "staggered") == 0 || strcmp(map->orientation, "hexagonal") == 0){
        unpk = json_unpack_ex(root,
                             &error,
                             0,
                             "{s:s, s:s}",
                             "staggeraxis", &map->staggeraxis,
                             "staggerindex", &map->staggerindex
                            );
        
        if(unpk == -1){
            logmsg(ERR, "Map '%s': Could not unpack map, %s at line %d, column %d", path, error.text, error.line, error.column);

            goto fail_map;
        }
    }

    if(strcmp(map->orientation, "hexagonal") == 0){
        unpk = json_unpack_ex(root, &error, 0, "{s:i}", "hexsidelength", &map->hexsidelength);

        if(unpk == -1){
            logmsg(ERR, "Map '%s': Could not unpack map, %s at line %d, column %d", path, error.text, error.line, error.column);

            goto fail_map;
        }
    }

    // Unpack properties
    unpk = json_unpack_ex(root, &error, 0, "{s?o}", "properties", &properties);

    if(unpk == -1){
        logmsg(ERR, "Map '%s': Could not unpack properties, %s at line %d, column %d", path, error.text, error.line, error.column);

        goto fail_map;
    }

    if(properties != NULL){
        map->properties = unpack_properties(properties);

        if(map->properties == NULL){
            logmsg(ERR, "Map '%s': Unable to unpack properties", path);

            goto fail_map;
        }
    }

    // Unpack layers
    unpk = json_unpack_ex(root, &error, 0, "{s:o}", "layers", &layers);

    if(unpk == -1){
        logmsg(ERR, "Map '%s': Could not unpack layers, %s at line %d, column %d", path, error.text, error.line, error.column);

        goto fail_properties;
    }

    map->layers = unpack_layers(layers);

    if(map->layers == NULL){
        logmsg(ERR, "Map '%s': Could not unpack layers", path);

        goto fail_properties;
    }

    map->layer_count = json_array_size(layers);

    // Unpack tilesets
    unpk = json_unpack_ex(root, &error, 0, "{s:o}", "tilesets", &tilesets);

    if(unpk == -1){
        logmsg(ERR, "Map '%s': Could not unpack tilesets, %s at line %d, column %d", path, error.text, error.line, error.column);

        goto fail_layers;
    }

    if(!json_is_array(tilesets)){
        logmsg(ERR, "Map '%s': Could not unpack tilesets, tilesets must be an array", path);

        goto fail_layers;
    }

    size_t tileset_count = json_array_size(tilesets);

    map->tilesets = calloc(tileset_count, sizeof(Tileset));

    if(map->tilesets == NULL){
        logmsg(ERR, "Map '%s': Could not unpack tilesets, the system is out of memory", path);

        goto fail_layers;
    }

    size_t idx;
    json_t* tileset = NULL;

    json_array_foreach(tilesets, idx, tileset){
        char* source = NULL;
        int firstgid = 0;

        char* tileset_path = NULL;

        unpk = json_unpack_ex(tileset, &error, 0, "{s?s, s?i}", "source", &source, "firstgid", &firstgid);

        if(unpk == -1){
            logmsg(ERR, "Map '%s': Could not unpack tilesets, %s at line %d column %d", path, error.text, error.line, error.column);

            goto fail_tilesets;
        }

        // If there's an external tileset, this will point to it, otherwise it'll point to the embedded tileset
        json_t* ts = tileset;

        // The tileset is not included in the map object, load it from the source before unpacking
        if(source){
            char* path_copy = strdup(path);

            char* dir = dirname(path_copy);

            tileset_path = calloc(1, strlen(dir) + strlen("/") + strlen(source));

            char* to = tileset_path;

            to = stpcpy(to, dir);
            to = stpcpy(to, "/");
            to = stpcpy(to, source);

            free(path_copy);

            ts = json_load_file(tileset_path, JSON_REJECT_DUPLICATES, &error);

            if(ts == NULL){
                logmsg(ERR, "Could not load map '%s', %s at line %d, column %d", path, error.text, error.line, error.column);

                free(tileset_path);

                goto fail_tilesets;
            }

            // If the tileset was loaded from an external source, the firstgid and source won't be present there, because the way Tiled serializes is retarded
            map->tilesets[idx].firstgid = firstgid;
            map->tilesets[idx].source = source;
        }

        if(!unpack_tileset(ts, &map->tilesets)){
            logmsg(ERR, "Map '%s': Could not unpack tileset at %s", path, tileset_path);

            free(tileset_path);

            goto fail_tilesets;
        }

        free(tileset_path);
    }


    return map;


fail_tilesets:
    free_tilesets(map->tilesets, map->tileset_count);

fail_layers:
    free_layers(map->layers, map->layer_count);

fail_properties:
    free(map->properties);

fail_map:
    json_decref(root);

    free(map);

    return NULL;
}

Map* map_load_file(const char* path, bool check_extension){
    char* ext = strrchr(path, '.');

    if(check_extension){
        if(ext == NULL){
            logmsg(ERR, "Map filename has no extension");

            return NULL;
        }

        if(strcmp(ext, ".tmj") != 0 && strcmp(ext, ".json") != 0){
            logmsg(ERR, "Map filename has unknown extension, '%s'", ext);
            logmsg(ERR, "Map filename must have '.tmj' or '.json' extension");

            return NULL;
        }
    }

    return map_load_json_file(path);
}

Map* map_load(const char* map){
    return NULL;
}
