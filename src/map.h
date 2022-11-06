#ifndef LIBTMJ_MAP
#define LIBTMJ_MAP

#include <jansson.h>

#include "tmj.h"

Property* unpack_properties(json_t* properties);
Object* unpack_objects(json_t* objects);
void free_objects(Object* objects, size_t object_count);

#endif
