#include "../include/map.h"
#include "Unity/src/unity.h"

void setUp(){}
void tearDown(){}

void test_map_load(){
    Map* m = map_load("../../example/overworld.tmj");
    TEST_ASSERT(m != NULL);
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_map_load);
    return UNITY_END();
}
