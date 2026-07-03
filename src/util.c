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

uint32_t* tmj_decode_layer(const char* data, const char* encoding, const char* compression, size_t* size) {
    if (strcmp(encoding, "base64") != 0) {
        logmsg(TMJ_LOG_ERR, "Layer data in csv format; decode it yourself");

        return NULL;
    }

    size_t dsize = 0;
    uint8_t* dat = tmj_b64_decode(data, &dsize);

    if (dat == NULL) {
        logmsg(TMJ_LOG_ERR, "Unable to base64 decode layer data");

        return NULL;
    }

    size_t dsize2 = 0;
    uint8_t* dat2 = NULL;

    if (strlen(compression) == 0) {
        dat2 = malloc(dsize);

        if (dat2 == NULL) {
            logmsg(TMJ_LOG_ERR, "Could not allocate buffer for decoded layer data, the system is out of memory");

            goto fail;
        }

        memcpy(dat2, dat, dsize);
        dsize2 = dsize;
    }

    if (strcmp(compression, "zlib") == 0 || strcmp(compression, "gzip") == 0) {
#ifndef LIBTMJ_ZLIB
        logmsg(TMJ_LOG_ERR, "Layer data encoded with %s, but libtmj was not compiled with %s support", compression, compression);

        goto fail;
#endif
// This ifdef is only here to shut up the compiler warnings
#ifdef LIBTMJ_ZLIB
        dat2 = tmj_zlib_decompress(dat, dsize, &dsize2);
#endif
    }

    if (strcmp(compression, "zstd") == 0) {
#ifndef LIBTMJ_ZSTD
        logmsg(TMJ_LOG_ERR, "Layer data encoded with zstd, but libtmj was not compiled with zstd support");

        goto fail;
#endif
#ifdef LIBTMJ_ZSTD
        dat2 = tmj_zstd_decompress(dat, dsize, &dsize2);
#endif
    }

    if (dat2 == NULL) {
        logmsg(TMJ_LOG_ERR, "Unable to decompress %s encoded layer data", compression);

        goto fail;
    }

    free(dat);

    *size = dsize2 / 4;

    return (uint32_t*)dat2;

fail:
    free(dat);
    return NULL;
}
