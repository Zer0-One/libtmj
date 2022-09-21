#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "../include/tmj.h"

/**
 * @file
 */

#define LOGMSG_BUFSIZE 1024

bool log_debug = false;
void (*log_callback)(log_priority, const char*) = NULL;

char logmsg_buf[LOGMSG_BUFSIZE];

void log_regcb(bool debug, void (*callback)(log_priority, const char*)){
    log_debug = debug;
    log_callback = callback;
}

void logmsg(log_priority priority, char* msg, ...){
    // Don't bother logging if there's no callback registered
    if(log_callback == NULL){
        return;
    }

    // Don't log debug messages if we have debugging turned off
    if(priority == DEBUG && !log_debug){
        return;
    }

    va_list args;

    va_start(args, msg);

    vsnprintf(logmsg_buf, LOGMSG_BUFSIZE, msg, args);

    va_end(args);

    log_callback(priority, logmsg_buf);
}
