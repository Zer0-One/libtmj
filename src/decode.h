#ifndef LIBTMJ_DECODE
#define LIBTMJ_DECODE

#include <stdint.h>

/**
 * @defgroup decode Decode
 *
 * Encoding/decoding routines.
 */

#ifdef LIBTMJ_ZSTD

#include <zstd.h>

/**
 * @ingroup decode
 * Decompresses a zstd-compressed array of bytes.
 *
 * @param data A zstd-compressed array of unsigned bytes.
 *
 * @return On success, returns a dynamically-allocated array of unsigned bytes.
 * The returned array must be freed by the caller.
 */
uint8_t* zstd_decompress(const uint8_t* data);

#endif

#ifdef LIBTMJ_ZLIB

#include <zlib.h>

/**
 * @ingroup decode
 * Decompresses a zlib-compressed array of bytes.
 *
 * @param data A zlib-compressed array of unsigned bytes.
 *
 * @return On success, returns a dynamically-allocated array of unsigned bytes.
 * The returned array must be freed by the caller.
 */
uint8_t* zlib_decompress(const uint8_t* data);

#endif

/**
 * @ingroup decode
 * Decodes a base64 string.
 *
 * @param data A valid base64 string.
 *
 * @return On success, returns a dynamically-allocated array of unsigned bytes.
 * The returned array must be freed by the caller.
 */
uint8_t* b64_decode(const char* data);


#endif
