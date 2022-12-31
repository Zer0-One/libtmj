#include <stdio.h>

#include "../include/tmj.h"

#include "Unity/src/unity.h"

void log_cb(tmj_log_priority priority, const char* msg){
    switch(priority){
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

void setUp(void){
    tmj_log_regcb(true, log_cb);
}

void tearDown(void){}

char* tileset_path = "example/overworld.tsj";

Tileset* tf = NULL;
Tileset* ts = NULL;

void test_tileset_loadf(void){
    tf = tmj_tileset_loadf(tileset_path, true);
    TEST_ASSERT_NOT_NULL(tf);
}

void test_tileset_load(void){
    FILE* f = fopen(tileset_path, "rb");

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);

    char* s = calloc(1, fsize + 1);

    TEST_ASSERT_EQUAL_size_t(fsize, fread(s, 1, fsize, f));
    fclose(f);

    ts = tmj_tileset_load(s);

    TEST_ASSERT_NOT_NULL(ts);

    free(s);
}

void test_tileset_free(void){
    tmj_tileset_free(tf);
    tmj_tileset_free(ts);
}

int main(void){
    UNITY_BEGIN();
    RUN_TEST(test_tileset_loadf);
    RUN_TEST(test_tileset_load);
    RUN_TEST(test_tileset_free);
    return UNITY_END();
}
