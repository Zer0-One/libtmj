#include <string.h>

#include "../include/tmj.h"
#include "../src/decode.h"

#include "Unity/src/unity.h"

void log_cb(log_priority priority, const char* msg){
    printf("TEST LOG: %s\n", msg);
}

void setUp(){
    log_regcb(true, log_cb);
}
void tearDown(){}

void test_b64_decode(){
    const char* msg = "VGhpcyBpcyBhIHRlc3Qgc3RyaW5n";
    const char* msg2 = "VGhpcyBpcyBhbm90aGVyIHRlc3Qgc3RyaW5nIQ==";

    size_t dSize;

    TEST_ASSERT(strncmp(b64_decode(msg, &dSize), "This is a test string", 21) == 0);
    TEST_ASSERT(strncmp(b64_decode(msg2, &dSize), "This is another test string!", 28) == 0);
}

void test_zlib_decode(){
    const char* msg_zlib = "eJwLycgsVgCiRIWS1OISheKSosy8dABRSQet";
    const char* msg_gzip = "H4sIAAAAAAAAAwvJyCxWAKJEhZLU4hKF4pKizLx0AG3zTmsVAAAA";

    size_t dSize;
    size_t decompressed_size_zlib = 0;
    size_t decompressed_size_gzip = 0;
    
    char* msg_zlib_decompressed = zlib_decompress(b64_decode(msg_zlib, &dSize), 37, &decompressed_size_zlib);
    char* msg_gzip_decompressed = zlib_decompress(b64_decode(msg_gzip, &dSize), 53, &decompressed_size_gzip);

    TEST_ASSERT(strncmp(msg_zlib_decompressed, "This is a test string", 21) == 0);
    TEST_ASSERT(strncmp(msg_gzip_decompressed, "This is a test string", 21) == 0);
    TEST_ASSERT(decompressed_size_zlib == 21);
    TEST_ASSERT(decompressed_size_gzip == 21);
}

void test_zstd_decode(){
    const char* msg = "KLUv/SQVqQAAVGhpcyBpcyBhIHRlc3Qgc3RyaW5nQBJZLg==";

    size_t dSize = 0;
    size_t decompressed_size_zstd = 0;

    uint8_t* buf = b64_decode(msg, &dSize);

    char* msg_decompressed = zstd_decompress(buf, dSize, &decompressed_size_zstd);

    TEST_ASSERT(strncmp(msg_decompressed, "This is a test string", 21) == 0);
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_b64_decode);
    RUN_TEST(test_zlib_decode);
    RUN_TEST(test_zstd_decode);
    return UNITY_END();
}
