#include <stdio.h>

#include "../include/tmj.h"

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

char* testmap_path = "example/overworld.tmj";
char* testmap_path2 = "example/testmap.tmj";

Map* mf = NULL;
Map* mf2 = NULL;
Map* ms = NULL;

void test_map_loadf(void) {
    mf = tmj_map_loadf(testmap_path, true);
    mf2 = tmj_map_loadf(testmap_path2, true);
    TEST_ASSERT_NOT_NULL(mf);
    TEST_ASSERT_NOT_NULL(mf2);
    TEST_ASSERT_EQUAL_size_t(4, mf->layer_count);
    TEST_ASSERT_EQUAL_STRING("tilelayer", mf->layers[0].type);
    TEST_ASSERT_EQUAL_STRING("bar", mf2->properties[0].value_string);
    TEST_ASSERT_EQUAL_INT(1, mf2->properties[1].value_object);
}

void test_map_load(void) {
    FILE* f = fopen(testmap_path, "rb");

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);

    char* s = calloc(1, fsize + 1);

    TEST_ASSERT_EQUAL_size_t(fread(s, 1, fsize, f), fsize);
    fclose(f);

    ms = tmj_map_load(s, "overworld");

    TEST_ASSERT_NOT_NULL(ms);
    TEST_ASSERT_EQUAL_size_t(4, ms->layer_count);
    TEST_ASSERT_EQUAL_STRING("tilelayer", ms->layers[0].type);

    free(s);
}

void test_map_free(void) {
    tmj_map_free(mf);
    tmj_map_free(mf2);
    tmj_map_free(ms);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_map_loadf);
    RUN_TEST(test_map_load);
    RUN_TEST(test_map_free);
    return UNITY_END();
}
