#include "../include/tmj.h"

#include "Unity/src/unity.h"

void log_cb(log_priority priority, const char* msg){
    printf("TEST LOG: %s\n", msg);
}

void setUp(){
    log_regcb(true, log_cb);
}
void tearDown(){}

void test_map_load(){
    Map* m = map_load_file("../../example/overworld.tmj", true);
    TEST_ASSERT(m != NULL);
}


int main(){
    UNITY_BEGIN();
    RUN_TEST(test_map_load);
    return UNITY_END();
}
