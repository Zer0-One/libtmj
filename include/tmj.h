#ifndef LIBTMJ_MAP
#define LIBTMJ_MAP

#include <stdbool.h>
#include <stdint.h>

#include <jansson.h>

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#property
 */
typedef struct Property {
    char* name;
    char* propertytype; //"When applicable", whatever the fuck that means
    char* type;

    json_t* value; //To-Do: parse this out into an actual C value.
    //union{
    //    char* string_value;
    //    int int_value;
    //    double float_value;
    //    bool bool_value;
    //    char* color_value;
    //    char* file_value;
    //    int object_value;
    //};
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

    union{
        char* data_str;
        unsigned int* data_uint;
    };
} Chunk;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#point
 */
typedef struct Point{
    double x;
    double y;
} Point;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#text
 */
typedef struct Text{
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

typedef enum OBJECT_TYPE{
    OBJECT,
    ELLIPSE,
    RECTANGLE,
    POINT,
    POLYGON,
    POLYLINE,
    TEXT
} object_type;

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#object
 */
typedef struct Object {
    object_type type;

    bool ellipse;
    bool point;
    bool visible;

    char* class; // Optional
    char* name;
    char* template;

    int gid;
    int id;

    double height;
    double rotation;
    double width;
    double x;
    double y;

    bool is_polygon;

    union{
        size_t polygon_point_count;
        size_t polyline_point_count;
    };
    union{
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
    union{
        unsigned int* data_uint;
        char* data_str;
    }; //tilelayer only

    int height; // tilelayer only
    int id;
    int startx;
    int starty;
    int width; //tilelayer only
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
    char* class; // Optional
    char* image; // Optional

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

    size_t terrain_count;
    Terrain* terrain; // Optional

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
 * Loads the Tiled map at the given path. The map object returned by this
 * function must not be modified by the caller, or undefined behavior may
 * result.
 *
 * \return On success, returns a pointer to a map. The map is
 * dynamically-allocated, and must be freed by the caller using map_free(). On
 * failure, returns NULL.
 */
Map* map_load(const char* path);

/**
 * Loads the Tiled tileset at the given path. The tileset object returned by
 * this function must not be modified by the caller, or undefined behavior may
 * result.
 *
 * \return On success, returns a pointer to a tileset. The tileset is
 * dynamically-allocated, and must be freed by the caller using tileset_free().
 * On failure, returns NULL.
 */
Tileset* tileset_load(const char* path);

/**
 * Frees the memory associated with the given map. This function will not free
 * any associated tilesets, those must be freed separately.
 */
void map_free(Map* map);

/**
 * Frees the memory associated with the given tileset.
 */
void tileset_free(Tileset* tileset);

#endif
