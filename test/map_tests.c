#include <stdio.h>

#include "../include/tmj.h"

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

char* testmap_path = "example/overworld.tmj";
size_t layer_count = 4;
char* layer1_type = "tilelayer";

Map* mf = NULL;
Map* ms = NULL;

void test_map_loadf(){
    mf = tmj_map_loadf(testmap_path, true);
    TEST_ASSERT_NOT_NULL(mf);
    TEST_ASSERT_EQUAL_size_t(mf->layer_count, 4);
    TEST_ASSERT_EQUAL_STRING(mf->layers[0].type, layer1_type);
}

void test_map_load(){
    FILE* f = fopen(testmap_path, "r");

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);

    char* s = calloc(1, fsize + 1);

    TEST_ASSERT_EQUAL_size_t(fread(s, 1, fsize, f), fsize);
    fclose(f);

    ms = tmj_map_load(s, "overworld");

    TEST_ASSERT_NOT_NULL(ms);
    TEST_ASSERT_EQUAL_size_t(ms->layer_count, 4);
    TEST_ASSERT_EQUAL_STRING(ms->layers[0].type, layer1_type);

    free(s);
}

void test_map_free(){
    tmj_map_free(mf);
    tmj_map_free(ms);
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_map_loadf);
    RUN_TEST(test_map_load);
    RUN_TEST(test_map_free);
    return UNITY_END();
}
