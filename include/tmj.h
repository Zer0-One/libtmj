#ifndef LIBTMJ_API
#define LIBTMJ_API

#include <stdbool.h>
#include <stdint.h>

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief The libtmj API.
 *
 * The structures returned by the functions defined in this interface are not
 * meant to be modified by the user. Use only the provided functions to do so.
 */

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#property
 */
typedef struct Property {
    char* name;
    char* propertytype;
    char* type;

    union {
        char* value_string;
        int value_int;
        double value_float;
        bool value_bool;
        char* value_color;
        char* value_file;
        int value_object;
    };
} Property;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#chunk
 */
typedef struct Chunk {
    int height;
    int width;
    int x;
    int y;

    bool data_is_str;
    size_t data_count;

    union {
        char* data_str;
        unsigned int* data_uint;
    };
} Chunk;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#point
 */
typedef struct Point {
    double x;
    double y;
} Point;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#text
 */
typedef struct Text {
    bool bold;
    bool italic;
    bool kerning;
    bool strikeout;
    bool underline;
    bool wrap;

    char* color;
    char* fontfamily;
    char* halign;
    char* text;
    char* valign;

    int pixelsize;
} Text;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#object
 */
typedef struct Object {
    bool ellipse;
    bool point;
    bool visible;

    char* name;
    char* template;
    char* type; // Optional

    int gid;
    int id;

    double height;
    double rotation;
    double width;
    double x;
    double y;

    bool is_polygon;

    union {
        size_t polygon_point_count;
        size_t polyline_point_count;
    };
    union {
        Point* polygon;
        Point* polyline;
    };

    size_t property_count;
    Property* properties;

    Text* text;
} Object;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#layer
 */
typedef struct Layer {
    bool locked;
    bool repeatx; // Imagelayer only
    bool repeaty; // Imagelayer only
    bool visible;

    char* class; // Optional
    char* compression; // tilelayer only
    char* draworder; // objectgroup only
    char* encoding; // tilelayer only
    char* image; // imagelayer only
    char* name;
    char* tintcolor; // Optional
    char* transparentcolor; // Optional, imagelayer only
    char* type;

    bool data_is_str;
    size_t data_count;
    union {
        unsigned int* data_uint;
        char* data_str;
    }; // tilelayer only

    int height; // tilelayer only
    int id;
    int imageheight;
    int imagewidth;
    int startx;
    int starty;
    int width; // tilelayer only
    int x;
    int y;

    double offsetx;
    double offsety;
    double opacity;
    double parallaxx;
    double parallaxy;

    size_t chunk_count;
    Chunk* chunks; // Optional, tilelayer only

    size_t layer_count;
    struct Layer* layers; // group only

    size_t object_count;
    Object* objects; // objectgroup only

    size_t property_count;
    Property* properties;
} Layer;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#transformations
 */
typedef struct Transformations {
    bool hflip;
    bool preferuntransformed;
    bool rotate;
    bool vflip;
} Transformations;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#terrain
 */
typedef struct Terrain {
    char* name;

    int tile;

    size_t property_count;
    Property* properties;
} Terrain;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#frame
 */
typedef struct Frame {
    int duration;
    int tileid;
} Frame;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#tile-definition
 */
typedef struct Tile {
    char* image; // Optional
    char* type; // Optional

    int id;
    int imageheight;
    int imagewidth;
    int x;
    int y;
    int width;
    int height;

    double probability; // Optional

    size_t animation_count;
    Frame* animation;

    int terrain[4]; // Optional

    size_t property_count;
    Property* properties;

    Layer* objectgroup; // Optional
} Tile;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#wang-tile
 */
typedef struct WangTile {
    int tileid;

    unsigned char wangid[8];
} WangTile;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#wang-color
 */
typedef struct WangColor {
    char* class;
    char* color;
    char* name;

    int tile;

    double probability;

    size_t property_count;
    Property* properties;
} WangColor;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#wang-set
 */
typedef struct WangSet {
    char* class; // Optional
    char* name;
    char* type;

    int tile;

    size_t color_count;
    WangColor* colors;

    size_t property_count;
    Property* properties;

    size_t wangtile_count;
    WangTile* wangtiles;
} WangSet;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#grid
 */
typedef struct Grid {
    char* orientation;

    int height;
    int width;
} Grid;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#tile-offset
 */
typedef struct TileOffset {
    int x;
    int y;
} TileOffset;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#tileset
 *
 * Note that for sheets, tiles are numbered left-to-right, top-to-bottom
 */
typedef struct Tileset {
    /**
     * The root object returned by jansson after parsing. This field is
     * internal state and should not be tampered with.
     */
    json_t* root; // NULL for embedded tilesets; only set for external tilesets

    char* backgroundcolor; // Optional
    char* class; // Optional
    char* fillmode;
    char* image;
    char* name;
    char* objectalignment;
    char* source;
    char* tiledversion;
    char* tilerendersize;
    char* transparentcolor; // Optional
    char* type;
    char* version;

    int columns;
    int firstgid;
    int imageheight;
    int imagewidth;
    int margin;
    int spacing;
    int tilecount;
    int tileheight;
    int tilewidth;

    size_t property_count;
    Property* properties;

    size_t terrain_count;
    Terrain* terrains; // Optional

    size_t tile_count;
    Tile* tiles; // Optional

    size_t wang_set_count;
    WangSet* wangsets;

    Grid* grid; // Optional

    TileOffset* tileoffset; // Optional

    Transformations* transformations; // Optional
} Tileset;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#json-map-format
 */
typedef struct Map {
    /**
     * The root object returned by jansson after parsing. This field is
     * internal state and should not be tampered with.
     */
    json_t* root;

    bool infinite;

    char* backgroundcolor; // Optional
    char* class; // Optional
    char* orientation;
    char* renderorder; // Orthogonal maps only
    char* staggeraxis; // Staggered/hexagonal maps only
    char* staggerindex; // Staggered/hexagonal maps only
    char* tiledversion;
    char* type;
    char* version;

    int compressionlevel;
    int height;
    int hexsidelength; // Hexagonal maps only
    int nextlayerid;
    int nextobjectid;
    int tileheight;
    int tilewidth;
    int width;

    double parallaxoriginx;
    double parallaxoriginy;

    // Layer tree
    size_t layer_count;
    Layer* layers;

    size_t property_count;
    Property* properties;

    size_t tileset_count;
    Tileset* tilesets;
} Map;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#object-template
 */
typedef struct ObjectTemplate {
    char* type;

    Tileset* tileset; // Optional
    Object* object;
} ObjectTemplate;

/**
 * @defgroup tmj TMJ
 *
 * Public API for loading JSON-formatted Tiled maps and tilesets.
 */

/**
 * @ingroup tmj
 * Loads the Tiled map from the file at the given path.
 *
 * The map object returned by this function must not be modified by the caller.
 *
 * @param path A relative or absolute filesystem path.
 * @param check_extension If true, validates that the file extension equals ".tmj" or ".json".
 *
 * @return On success, returns a pointer to a map. The map is
 * dynamically-allocated, and must be freed by the caller using map_free(). On
 * failure, returns NULL.
 */
Map* tmj_map_loadf(const char* path, bool check_extension);

/**
 * @ingroup tmj
 * Loads the Tiled map from the given JSON object string.
 *
 * The map object returned by this function must not be modified by the caller.
 *
 * @param map A JSON string containing a Tiled map object.
 * @param name A name to use to reference this map in log messages.
 * tmj_map_loadf() does not require this argument, because it uses the file
 * path to identify the map in log messages.
 *
 * @return On success, returns a pointer to a map. The map is
 * dynamically-allocated, and must be freed by the caller using map_free(). On
 * failure, returns NULL.
 */

Map* tmj_map_load(const char* map, const char* name);

/**
 * @ingroup tmj
 * Loads the Tiled tileset at the given path. The tileset object returned by
 * this function must not be modified by the caller.
 *
 * @param path A relative or absolute filesystem path.
 * @param check_extension If true, validates that the file extension equals ".tsj" or ".json".
 *
 * @return On success, returns a pointer to a tileset. The tileset is
 * dynamically-allocated, and must be freed by the caller using tileset_free().
 * On failure, returns NULL.
 */
Tileset* tmj_tileset_loadf(const char* path, bool check_extension);

/**
 * @ingroup tmj
 * Loads the Tiled tileset from the given JSON object string. The tileset
 * object returned by this function must not be modified by the caller.
 *
 * @param tileset A JSON string containing a Tiled tileset object.
 *
 * @return On success, returns a pointer to a tileset. The tileset is
 * dynamically-allocated, and must be freed by the caller using tileset_free().
 * On failure, returns NULL.
 */
Tileset* tmj_tileset_load(const char* tileset);

/**
 * @ingroup tmj
 * Frees the memory associated with the given map.
 *
 * If the map was modified by the caller, this function may cause undefined
 * behavior.
 *
 * @param map A map which was returned by a call to map_load().
 */
void tmj_map_free(Map* map);

/**
 * @ingroup tmj
 * Frees the memory associated with the given tileset. If the tileset was
 * modified by the caller, this function may cause undefined behavior.
 *
 * @param tileset A tileset which was returned by a call to tileset_load().
 */
void tmj_tileset_free(Tileset* tileset);

/**
 * @ingroup tmj
 */
typedef enum TMJ_LOG_PRIORITY {
    TMJ_LOG_DEBUG,
    TMJ_LOG_INFO,
    TMJ_LOG_WARNING,
    TMJ_LOG_ERR,
    TMJ_LOG_CRIT
} tmj_log_priority;

/**
 * @ingroup tmj
 * Registers a callback function to handle logging events.
 *
 * @param debug If set to true, the given callback function will receive debug
 * messages and information in addition to the higher-priority messages.
 * @param callback A function that takes a TMJ_LOG_PRIORITY and a char* and
 * returns nothing.
 */
void tmj_log_regcb(bool debug, void (*callback)(tmj_log_priority, const char*));

///**
// * @defgroup util Util
// *
// * Helper functions.
// */

/**
 * @ingroup tmj
 * The library major version.
 *
 * An unsigned integer representing the major component of the SemVer 2.0
 * version of this library.
 */
extern const unsigned int TMJ_VERSION_MAJOR;

/**
 * @ingroup tmj
 * The library minor version.
 *
 * An unsigned integer representing the minor component of the SemVer 2.0
 * version of this library.
 */
extern const unsigned int TMJ_VERSION_MINOR;

/**
 * @ingroup tmj
 * The library patch version.
 *
 * An unsigned integer representing the patch component of the SemVer 2.0
 * version of this library.
 */
extern const unsigned int TMJ_VERSION_PATCH;

/**
 * @ingroup tmj
 * The full library version.
 *
 * A string containing the full SemVer 2.0 version of this library.
 */
extern const char* const TMJ_VERSION;

/**
 * @ingroup tmj
 * Decodes layer data from a Tiled map layer.
 *
 * @param data The value of the "data_str" field from a Layer.
 * @param encoding The value of the "encoding" field from a Layer.
 * @param compression The value of the "compression" field from a Layer.
 * @param[out] size The size of the resultant integer array.
 *
 * @return On Success, returns a dynamically-allocated array of global tile
 * IDs. This array must be freed by the caller. See
 * https://doc.mapeditor.org/en/stable/reference/global-tile-ids/ for more
 * information. On failure, returns NULL.
 */
uint32_t* tmj_decode_layer(const char* data, const char* encoding, const char* compression, size_t* size);

#ifdef __cplusplus
}
#endif

#endif
