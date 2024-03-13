#include <string.h>

#include "../include/tmj.h"
#include "../src/decode.h"

#include "Unity/src/unity.h"

void log_cb(tmj_log_priority priority, const char* msg) {
    switch (priority) {
        case TMJ_LOG_DEBUG:
            printf("DEBUG: %s\n", msg);
            break;
        case TMJ_LOG_INFO:
            printf("INFO: %s\n", msg);
            break;
        case TMJ_LOG_WARNING:
            printf("WARNING: %s\n", msg);
            break;
        case TMJ_LOG_ERR:
            printf("ERR: %s\n", msg);
            break;
        case TMJ_LOG_CRIT:
            printf("CRIT: %s\n", msg);
            break;
    }
}

void setUp(void) {
    tmj_log_regcb(true, log_cb);
}
void tearDown(void) {}

void test_b64_decode(void) {
    const char* msg = "VGhpcyBpcyBhIHRlc3Qgc3RyaW5nAA==";
    const char* msg2 = "VGhpcyBpcyBhbm90aGVyIHRlc3Qgc3RyaW5nIQA=";

    size_t dSize;

    uint8_t* out = tmj_b64_decode(msg, &dSize);

    TEST_ASSERT_EQUAL_INT(22, dSize);

    uint8_t* out2 = tmj_b64_decode(msg2, &dSize);

    TEST_ASSERT_EQUAL_INT(29, dSize);

    TEST_ASSERT_EQUAL_STRING("This is a test string", out);
    TEST_ASSERT_EQUAL_STRING("This is another test string!", out2);

    free(out);
    free(out2);
}

#ifdef LIBTMJ_ZLIB
void test_zlib_decode(void) {
    const char* msg_zlib = "eJwLycgsVgCixLz8kozUIoWS1OISheKSosy8dEUGAKBMCl4=";
    const char* msg_gzip = "H4sIAAAAAAAAAwvJyCxWAKLEvPySjNQihZLU4hKF4pKizLx0RQYAPCsnLx0AAAA=";

    size_t dSize;
    size_t decompressed_size_zlib = 0;
    size_t decompressed_size_gzip = 0;

    uint8_t* out = tmj_b64_decode(msg_zlib, &dSize);
    TEST_ASSERT_NOT_NULL(out);

    char* msg_zlib_decompressed = tmj_zlib_decompress(out, dSize, &decompressed_size_zlib);

    free(out);

    out = tmj_b64_decode(msg_gzip, &dSize);
    TEST_ASSERT_NOT_NULL(out);

    char* msg_gzip_decompressed = tmj_zlib_decompress(out, dSize, &decompressed_size_gzip);

    free(out);

    TEST_ASSERT_EQUAL_INT(29, decompressed_size_zlib); // 28 characters + 1 trailing NULL
    TEST_ASSERT_EQUAL_INT(29, decompressed_size_gzip);
    TEST_ASSERT_EQUAL_STRING("This is another test string!", msg_zlib_decompressed);
    TEST_ASSERT_EQUAL_STRING("This is another test string!", msg_gzip_decompressed);

    free(msg_zlib_decompressed);
    free(msg_gzip_decompressed);
}
#endif

#ifdef LIBTMJ_ZSTD
void test_zstd_decode(void) {
    const char* msg = "KLUv/SQd6QAAVGhpcyBpcyBhbm90aGVyIHRlc3Qgc3RyaW5nIQDZN/8V";

    size_t dSize = 0;
    size_t decompressed_size_zstd = 0;

    uint8_t* out = tmj_b64_decode(msg, &dSize);

    char* msg_decompressed = tmj_zstd_decompress(out, dSize, &decompressed_size_zstd);

    TEST_ASSERT_EQUAL_INT(29, decompressed_size_zstd);
    TEST_ASSERT_EQUAL_STRING("This is another test string!", msg_decompressed);

    free(out);
    free(msg_decompressed);
}
#endif

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_b64_decode);
#ifdef LIBTMJ_ZLIB
    RUN_TEST(test_zlib_decode);
#endif
#ifdef LIBTMJ_ZSTD
    RUN_TEST(test_zstd_decode);
#endif
    return UNITY_END();
}
