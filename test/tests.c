#include "../include/map.h"
#include "Unity/src/unity.h"

void setUp(){}
void tearDown(){}

void testtest(){
    TEST_ASSERT(2 + 2 == 4);
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(testtest);
    return UNITY_END();
}
