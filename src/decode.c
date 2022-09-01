#include "decode.h"

#ifdef LIBTMJ_ZSTD

uint8_t* zstd_decompress(const uint8_t* data){
    return NULL;
}

#endif

#ifdef LIBTMJ_ZLIB

uint8_t* zlib_decompress(const uint8_t* data){
    return NULL;
}

#endif

uint8_t* b64_decode(const char* data){
    return NULL;
}
