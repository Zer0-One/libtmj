#include <string.h>

#include "../include/tmj.h"
#include "../src/decode.h"

#include "Unity/src/unity.h"

void log_cb(tmj_log_priority priority, const char* msg){
    switch(priority){
        case DEBUG:
            printf("DEBUG: %s\n", msg);
            break;
        case INFO:
            printf("INFO: %s\n", msg);
            break;
        case WARNING:
            printf("WARNING: %s\n", msg);
            break;
        case ERR:
            printf("ERR: %s\n", msg);
            break;
        case CRIT:
            printf("CRIT: %s\n", msg);
            break;
    }
}

void setUp(){
    tmj_log_regcb(true, log_cb);
}
void tearDown(){}

void test_b64_decode(){
    const char* msg = "VGhpcyBpcyBhIHRlc3Qgc3RyaW5n";
    const char* msg2 = "VGhpcyBpcyBhbm90aGVyIHRlc3Qgc3RyaW5nIQ==";

    size_t dSize;

    TEST_ASSERT_EQUAL_STRING(tmj_b64_decode(msg, &dSize), "This is a test string");
    TEST_ASSERT_EQUAL_STRING(tmj_b64_decode(msg2, &dSize), "This is another test string!");
}

#ifdef LIBTMJ_ZLIB
void test_zlib_decode(){
    const char* msg_zlib = "eJwLycgsVgCiRIWS1OISheKSosy8dABRSQet";
    const char* msg_gzip = "H4sIAAAAAAAAAwvJyCxWAKJEhZLU4hKF4pKizLx0AG3zTmsVAAAA";

    size_t dSize;
    size_t decompressed_size_zlib = 0;
    size_t decompressed_size_gzip = 0;
    
    char* msg_zlib_decompressed = tmj_zlib_decompress(tmj_b64_decode(msg_zlib, &dSize), 37, &decompressed_size_zlib);
    char* msg_gzip_decompressed = tmj_zlib_decompress(tmj_b64_decode(msg_gzip, &dSize), 53, &decompressed_size_gzip);

    TEST_ASSERT_EQUAL_STRING(msg_zlib_decompressed, "This is a test string");
    TEST_ASSERT_EQUAL_STRING(msg_gzip_decompressed, "This is a test string");
    TEST_ASSERT_EQUAL_INT(decompressed_size_zlib, 21);
    TEST_ASSERT_EQUAL_INT(decompressed_size_gzip, 21);
}
#endif

#ifdef LIBTMJ_ZSTD
void test_zstd_decode(){
    const char* msg = "KLUv/SQVqQAAVGhpcyBpcyBhIHRlc3Qgc3RyaW5nQBJZLg==";

    size_t dSize = 0;
    size_t decompressed_size_zstd = 0;

    uint8_t* buf = tmj_b64_decode(msg, &dSize);

    char* msg_decompressed = tmj_zstd_decompress(buf, dSize, &decompressed_size_zstd);

    TEST_ASSERT_EQUAL_STRING(msg_decompressed, "This is a test string");
}
#endif

int main(){
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
