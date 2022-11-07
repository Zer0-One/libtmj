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

char* tileset_path = "../../example/overworld.tsj";

Tileset* tf = NULL;
Tileset* ts = NULL;

void test_tileset_loadf(){
    tf = tmj_tileset_loadf(tileset_path, true);
    TEST_ASSERT_NOT_NULL(tf);
}

void test_tileset_load(){
    FILE* f = fopen(tileset_path, "r");

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);

    char* s = calloc(1, fsize + 1);

    TEST_ASSERT_EQUAL_size_t(fread(s, 1, fsize, f), fsize);
    fclose(f);

    ts = tmj_tileset_load(s);

    TEST_ASSERT_NOT_NULL(ts);

    free(s);
}

void test_tileset_free(){
    tmj_tileset_free(tf);
    tmj_tileset_free(ts);
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_tileset_loadf);
    RUN_TEST(test_tileset_load);
    RUN_TEST(test_tileset_free);
    return UNITY_END();
}