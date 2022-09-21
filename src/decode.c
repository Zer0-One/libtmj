#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "decode.h"
#include "log.h"

#ifdef LIBTMJ_ZSTD

uint8_t* zstd_decompress(const uint8_t* data, size_t data_size, size_t* decompressed_size){
    logmsg(DEBUG, "Decode (zstd): Decompressing buffer of size %zu", data_size);

    if(data == NULL){
        logmsg(ERR, "Decode (zstd): Cannot decompress NULL buffer");

        return NULL;
    }

    size_t ret_size = ZSTD_getFrameContentSize(data, data_size);

    if(ret_size == ZSTD_CONTENTSIZE_ERROR){
        logmsg(ERR, "Decode (zstd): Unable to decompress non-zstd buffer");

        return NULL;
    }

    if(ret_size == ZSTD_CONTENTSIZE_UNKNOWN){
        logmsg(ERR, "Decode (zstd): Unable to determine uncompressed size of compressed data");

        return NULL;
    }

    void* ret = malloc(ret_size);

    if(ret == NULL){
        logmsg(ERR, "Decode (zstd): Unable to allocate buffer for decompressed data, the system is out of memory");

        return NULL;
    }

    size_t dsize = ZSTD_decompress(ret, ret_size, data, data_size);

    logmsg(DEBUG, "Decode (zstd): Decompressed byte total: %zu", dsize);

    if(ZSTD_isError(dsize)){
        logmsg(ERR, "Decode (zstd): Decompression error: %s", ZSTD_getErrorName(dsize));

        free(ret);

        return NULL;
    }

    *decompressed_size = ret_size;

    return ret;
}

#endif

#ifdef LIBTMJ_ZLIB

uint8_t* zlib_decompress(const uint8_t* data, size_t data_size, size_t* decompressed_size){
    logmsg(DEBUG, "Decode (zlib): Decompressing buffer of size %zu", data_size);

    if(data == NULL){
        logmsg(ERR, "Decode (zlib): Cannot decompress NULL buffer");

        return NULL;
    }

    const size_t INFLATE_BLOCK_SIZE = 262144;

    void* out = malloc(INFLATE_BLOCK_SIZE);

    if(out == NULL){
        logmsg(ERR, "Decode (zlib): Unable to allocate buffer for decompressed data, the system is out of memory");

        return NULL;
    }

    z_stream stream = {0};

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    stream.avail_in = data_size;
    stream.avail_out = data_size;

    stream.next_in = data;
    stream.next_out = out;

    // 15 + 32 for zlib and gzip decoding with automatic header detection, according to the manual
    int ret = inflateInit2(&stream, 15 + 32);

    switch(ret){
        case Z_OK:
            logmsg(DEBUG, "Decode (zlib): inflate initialization OK");
            break;

        case Z_MEM_ERROR:
            logmsg(ERR, "Decode (zlib): Unable to initialize inflate, the system is out of memory");

            goto fail_zlib;

        case Z_VERSION_ERROR:
            logmsg(ERR, "Decode (zlib): Unable to initialize inflate, incompatible zlib library version");

            goto fail_zlib;

        case Z_STREAM_ERROR:
            logmsg(ERR, "Decode (zlib): Unable to initialize inflate, invalid parameter(s) to inflate initialization routine");

            goto fail_zlib;

        default:
            goto fail_zlib;
    }

    size_t realloc_scale = 2;

    // Iteratively inflate, growing the output buffer by 1 block each time we run out of space
    //
    // Note from the manual: "If inflate returns Z_OK and with zero avail_out,
    // it must be called again after making room in the output buffer because
    // there might be more output pending."
    // Unsure if this is actually necessary, or if we can rely on Z_BUF_ERROR to tell us when to resize
    int stat = inflate(&stream, Z_NO_FLUSH);

    while(stat != Z_STREAM_END){
        switch(stat){
            case Z_BUF_ERROR:
                if(realloc(out, INFLATE_BLOCK_SIZE * realloc_scale) == NULL){
                    free(out);

                    logmsg(ERR, "Decode (zlib): Unable to grow inflate output buffer, the system is out memory");

                    return NULL;
                }

                realloc_scale++;

                stream.avail_out += INFLATE_BLOCK_SIZE;

                break;

            case Z_OK:
                logmsg(DEBUG, "Decode (zlib): inflate OK");

                break;

            case Z_NEED_DICT:
                logmsg(ERR, "Decode (zlib): Unable to complete inflate, preset dictionary required");

                goto fail_zlib;

            case Z_DATA_ERROR:
                logmsg(ERR, "Decode (zlib): Unable to complete inflate, input data appears corrupted");

                goto fail_zlib;

            case Z_STREAM_ERROR:
                logmsg(ERR, "Decode (zlib): Unable to complete inflate, stream structure inconsistent");

                goto fail_zlib;

            case Z_MEM_ERROR:
                logmsg(ERR, "Decode (zlib): Unable to complete inflate, the system is out of memory");

                goto fail_zlib;

            default:
                goto fail_zlib;
        }

        stat = inflate(&stream, Z_NO_FLUSH);
    }

    logmsg(DEBUG, "Decode (zlib): Completed inflate, %zd bytes written to output buffer", stream.total_out);

    if(inflateEnd(&stream) != Z_OK){
        logmsg(ERR, "Decode (zlib): Completed inflate, but could not clean up; stream state was inconsistent");

        goto fail_zlib;
    }

    *decompressed_size = stream.total_out;

    return out;

fail_zlib:
    free(out);

    if(stream.msg){
        logmsg(ERR, "Decode (zlib): zlib error: '%s'", stream.msg);
    }

    return NULL;
}

#endif

// Thanks to John's article for explaining Base64: https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/

const char b64_encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char b64_decode_table[] = { 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 62, 255, 255, 255, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255,
    255, 255, 255, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255, 255, 26,
    27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
    47, 48, 49, 50, 51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255
};

// Not used, but included here for reference, to explain how the decode table was generated
//void b64_generate_decode_table(){
//    memset(&b64_decode_table, -1, 256);
//
//    for(size_t i = 0; i < 256; i++){
//        b64_decode_table[b64_encode_table[i]] = i;
//    }
//}

size_t b64_decode_size(const char* data){
    if(data == NULL){
        return 0;
    }

    size_t len = strlen(data);

    size_t ret = len / 4 * 3;

    // Check to see if the last 2 characters are padding bytes
    if(data[len-1] == '='){
        ret--;

        if(data[len-2] == '='){
            ret--;
        }
    }

    return ret;
}

bool b64_is_valid_char(char c){
    if(isalnum(c)){
        return true;
    }

    if(c == '+' || c == '/' || c == '='){
        return true;
    }

    return false;
}

// Don't need this yet, so it'll stay unimplemented for now
char* b64_encode(uint8_t* data){
    return NULL;
}

uint8_t* b64_decode(const char* data, size_t* decoded_size){
    if(data == NULL){
        logmsg(ERR, "Decode (b64): Unable to decode null input");

        return NULL;
    }

    size_t len = strlen(data);

    if(len % 4 != 0){
        logmsg(ERR, "Decode (b64): Invalid Base64 string, input length is not a multiple of 4");

        return NULL;
    }

    size_t dSize = b64_decode_size(data);
    uint8_t* out = malloc(dSize);

    if(out == NULL){
        logmsg(ERR, "Decode (b64): Unable to allocate output buffer, the system is out of memory");

        free(out);

        return NULL;
    }

    // Validate the input
    for(size_t i = 0; i < len; i++){
        if(!b64_is_valid_char(data[i])){
            logmsg(ERR, "Decode (b64): Invalid Base64 character, '%c'", data[i]);

            free(out);

            return NULL;
        }
    }

    for(size_t i = 0, j = 0; i < len; i+=4, j+=3){
        // Pack decoded 6-bit values into an integer
        uint32_t p = b64_decode_table[data[i]];
        p = (p << 6) | b64_decode_table[data[i+1]];
        p = data[i+2] == '=' ? p << 6 : (p << 6) | b64_decode_table[data[i+2]];
        p = data[i+3] == '=' ? p << 6 : (p << 6) | b64_decode_table[data[i+3]];

        // Reinterpret into 8-bit bytes
        out[j] = (p >> 16) & 0xFF;
        if(data[i+2] != '='){
            out[j+1] = (p >> 8) & 0xFF;
        }
        if(data[i+3] != '='){
            out[j+2] = p & 0xFF;
        }
    }

    *decoded_size = dSize;

    return out;
}
