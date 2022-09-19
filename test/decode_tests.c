#include <string.h>

#include "../src/decode.h"
#include "Unity/src/unity.h"

void setUp(){}
void tearDown(){}

void test_b64_decode(){
    const char* msg = "VGhpcyBpcyBhIHRlc3Qgc3RyaW5n";
    const char* msg2 = "VGhpcyBpcyBhbm90aGVyIHRlc3Qgc3RyaW5nIQ==";

    TEST_ASSERT(strncmp(b64_decode(msg), "This is a test string", 21) == 0);
    TEST_ASSERT(strncmp(b64_decode(msg2), "This is another test string!", 28) == 0);
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_b64_decode);
    return UNITY_END();
}
