#ifndef LIBTMJ_TILESET
#define LIBTMJ_TILESET

#include "tmj.h"

int unpack_tileset(json_t* tileset, Tileset* ret);
void tilesets_free(Tileset* tilesets, size_t tileset_count);

#endif
