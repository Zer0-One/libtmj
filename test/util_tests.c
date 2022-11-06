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

void test_version(){
    TEST_ASSERT_EQUAL_UINT(TMJ_VERSION_MAJOR, 0);
    TEST_ASSERT_EQUAL_UINT(TMJ_VERSION_MINOR, 1);
    TEST_ASSERT_EQUAL_UINT(TMJ_VERSION_PATCH, 7);
    TEST_ASSERT_EQUAL_STRING(TMJ_VERSION, "0.1.0");
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_version);
    return UNITY_END();
}
