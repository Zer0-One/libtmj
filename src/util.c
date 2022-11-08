#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "decode.h"
#include "log.h"

// Library version definition
const unsigned int TMJ_VERSION_MAJOR = LIBTMJ_VERSION_MAJOR;
const unsigned int TMJ_VERSION_MINOR = LIBTMJ_VERSION_MINOR;
const unsigned int TMJ_VERSION_PATCH = LIBTMJ_VERSION_PATCH;
const char* const TMJ_VERSION = LIBTMJ_VERSION;

uint32_t* tmj_decode_layer(const char* data, const char* encoding, const char* compression, size_t* size){
    if(strcmp(encoding, "base64") != 0){
        logmsg(ERR, "Layer data in csv format; decode it yourself");

        return NULL;
    }

    size_t dsize = 0;
    uint8_t* dat = b64_decode(data, &dsize);

    if(dat == NULL){
        logmsg(ERR, "Unable to base64 decode layer data");

        return NULL;
    }

    size_t dsize2 = 0;
    uint8_t* dat2 = NULL;

    if(strcmp(compression, "zlib") == 0 || strcmp(compression, "gzip") == 0){
#ifndef LIBTMJ_ZLIB
        logmsg(ERR, "Layer data encoded with %s, but libtmj was not compiled with %s support", compression, compression);

        return NULL;
#endif
        dat2 = zlib_decompress(dat, dsize, &dsize2);
    }

    if(strcmp(compression, "zstd") == 0){
#ifndef LIBTMJ_ZSTD
        logmsg(ERR, "Layer data encoded with zstd, but libtmj was not compiled with zstd support");

        return NULL;
#endif
        dat2 = zstd_decompress(dat, dsize, &dsize2);
    }

    if(dat2 == NULL){
        logmsg(ERR, "Unable to decompress %s encoded layer data", compression);

        free(dat);

        return NULL;
    }

    free(dat);

    *size = dsize2 / 4;

    return dat2;
}
