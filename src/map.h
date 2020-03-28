#ifndef GAM_MAP
#define GAM_MAP

#include <stdbool.h>
#include <stdint.h>


/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#json-map-format
 */
struct map{
    bool infinite;

    char* backgroundcolor;
    char* orientation;
    char* renderorder;
    char* staggeraxis;
    char* staggerindex;
    char* tiledversion;
    char* type;

    int height;
    int hexsidelength;
    int nextlayerid;
    int nextobjectid;
    int tileheight;
    int tilewidth;
    int width;

    double version;

    size_t layer_count;
    struct layer* layers;

    size_t property_count;
    struct property* properties;

    size_t tileset_count;
    struct tileset** tilesets;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#layer
 */
struct layer{
    bool visible;

    char* compression;
    char* draworder;
    char* encoding;
    char* image;
    char* name;
    char* transparentcolor;
    char* type;

    int height;
    int id;
    int startx;
    int starty;
    int width;
    int x;
    int y;

    double offsetx;
    double offsety;
    double opacity;

    size_t chunk_count;
    struct chunk* chunks;

    size_t data_count;
    union{
        unsigned int** data_int;
        char* data_str;
    };

    size_t layer_count;
    struct layer* layers;

    size_t object_count;
    struct object* objects;

    size_t property_count;
    struct property* properties;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#chunk
 */
struct chunk{
    int height;
    int width;
    int x;
    int y;

    size_t data_count;
    union{
        unsigned int** data_int;
        char* data_str;
    };
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#object
 */
struct object{
    bool ellipse;
    bool point;
    bool visible;

    char* name;
    char* template;
    char* type;

    int gid;
    int id;

    double height;
    double rotation;
    double width;
    double x;
    double y;

    union{
        size_t polygon_point_count;
        size_t polyline_point_count;
    };
    union{
        struct point* polygon;
        struct point* polyline;
    };

    size_t property_count;
    struct property* properties;

    struct text* text;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#text
 */
struct text{
    bool bold;
    bool italic;
    bool kerning;
    bool underline;
    bool wrap;

    char* color;
    char* fontfamily;
    char* halign;
    char* text;
    char* valign;

    int pixelsize;
};

// For sheets, tiles are numbered left-to-right, top-to-bottom
/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#tileset
 */
struct tileset{
    char* backgroundcolor;
    char* image;
    char* name;
    char* source;
    char* tiledversion;
    char* transparentcolor;
    char* type;

    int columns;
    int firstgid;
    int imageheight;
    int imagewidth;
    int margin;
    int spacing;
    int tilecount;
    int tileheight;
    int tilewidth;

    double version;

    size_t property_count;
    struct property* properties;

    size_t terrain_count;
    struct terrain* terrains;

    size_t tile_count;
    struct tile* tiles;

    size_t wang_set_count;
    struct wang_set* wangsets;

    struct grid* grid;

    struct tile_offset* tileoffset;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#grid
 */
struct grid{
    char* orientation;

    int height;
    int width;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#tile-offset
 */
struct tile_offset{
    int x;
    int y;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#tile-definition
 */
struct tile{
    char* image;
    char* type;

    int id;
    int imageheight;
    int imagewidth;

    double probability;

    struct layer* layer;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#frame
 */
struct frame{
    int duration;
    int tileid;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#terrain
 */
struct terrain{
    char* name;

    int tile;

    size_t property_count;
    struct property* properties;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#wang-set
 */
struct wang_set{
    char* name;

    int tile;

    size_t cornercolor_count;
    struct wang_color* cornercolors;

    size_t edgecolor_count;
    struct wang_color* edgecolors;

    size_t property_count;
    struct property* properties;

    size_t wangtile_count;
    struct wang_tile* wang_tiles;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#wang-color
 */
struct wang_color{
    char* color;
    char* name;

    int tile;

    double probability;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#wang-tile
 */
struct wang_tile{
    bool dflip;
    bool hflip;
    bool vflip;

    int tileid;

    unsigned char wangid[8];
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#object-template
 */
struct object_template{
    char* type;

    size_t tileset_count;
    struct tileset* tilesets;

    size_t object_count;
    struct object* objects;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#property
 */
struct property{
    char* name;
    char* type;
    char* value;
};

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format/#point
 */
struct point{
    double x;
    double y;
};



enum layer_type{
    TILE = 0,
    OBJECT = 1
};

enum tileset_type{
    INDIVIDUAL = 0,
    SHEET = 1
};


struct sheet{
    char* path;
    int height_px;
    int width_px;
    int margin;
    int spacing;
    uint8_t* data;
};

/**
 * Loads the Tiled map at the given path.
 *
 * \return On success, a pointer to a map. The map is dynamically-allocated,
 * and must be freed by the caller.
 */
struct map* map_load(const char* path);

/**
 * Loads the Tiled tileset at the given path.
 *
 * \return On success, a pointer to a tileset. The tileset is
 * dynamically-allocated, and must be freed by the caller.
 */
struct tileset* tileset_load(const char* path);

/**
 * Frees the memory associated with the given map. This function will not free
 * any associated tilesets, those must be freed separately.
 */
void map_destroy(struct map* map);

/**
 * Frees the memory associated with the given tileset.
 */
void tileset_destroy(struct tileset* tileset);

#endif
