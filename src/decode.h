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
 * Decompresses a zstd-compressed buffer of bytes. This routine does not use
 * streaming decompression, so the given buffer must have a frame header that
 * includes the content size. See
 * https://github.com/facebook/zstd/blob/dev/doc/zstd_compression_format.md#frame_header
 * for detail.
 *
 * @param data A zstd-compressed buffer of unsigned bytes. The zstd frame
 * header for this buffer must include the content size.
 * @param data_size The length of the buffer.
 * @param[out] decompressed_size The length of the returned decompressed buffer.
 *
 * @return On success, returns a dynamically-allocated (and decompressed)
 * buffer of unsigned bytes. The returned buffer must be freed by the caller.
 */
uint8_t* tmj_zstd_decompress(const uint8_t* data, size_t data_size, size_t* decompressed_size);

#endif

#ifdef LIBTMJ_ZLIB

#include <zlib.h>

/**
 * @ingroup decode
 * Decompresses a zlib/gzip-compressed buffer of bytes.
 *
 * @param data A zlib/gzip-compressed buffer of unsigned bytes.
 * @param data_size The length of the buffer.
 * @param[out] decompressed_size The length of the returned decompressed buffer.
 *
 * @return On success, returns a dynamically-allocated (and decompressed)
 * buffer of unsigned bytes. The returned buffer must be freed by the caller.
 */
uint8_t* tmj_zlib_decompress(const uint8_t* data, size_t data_size, size_t* decompressed_size);

#endif

/**
 * @ingroup decode
 * Decodes a base64 string.
 *
 * @param data A null-terminated base64 string.
 * @param[out] decoded_size The length of the returned decoded buffer.
 *
 * @return On success, returns a dynamically-allocated array of unsigned bytes.
 * The returned array must be freed by the caller.
 */
uint8_t* tmj_b64_decode(const char* data, size_t* decoded_size);

#endif
