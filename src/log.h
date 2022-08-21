#ifndef LIBTMJ_LOG
#define LIBTMJ_LOG

#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>

/**
 * @file
 *
 * @defgroup logging
 *
 * Private logging API.
 */

/**
 * @ingroup logging
 * The logging facility to be used for messages being logged to the syslog.
 */
#define LOG_FACILITY LOG_USER

/**
 * @ingroup logging
 * If this variable is set to true, logs will include debug messages.
 */
extern bool debug;

/**
 * @ingroup logging
 * If this variable is set to true, logs will be written to stdout instead of
 * the syslog.
 */
extern bool foreground;

/**
 * @ingroup logging
 * Logs messages to the foreground. This function should not be used directly
 * under normal circumstances. Use the logmsg() macro instead.
 *
 * @param loglevel One of the set of log priorities defined in <syslog.h>. Do
 *                 \b not include the logging facility. This function will use the logging
 *                 facility defined by LOG_FACILITY by default.
 * @param msg      A printf-style format string for the message to be logged
 * @param ...      Format string arguments for the previous argument. See printf() for detail.
 */
void logprintf(int loglevel, char* msg, ...);

/**
 * @ingroup logging
 * This macro defines the logic for choosing whether to log messages to the
 * syslog, or to stdout. Use this macro wherever a message needs to be logged.
 *
 * The expected arguments are identical to that of logprintf().
 */
#define logmsg(loglevel, ...) do{\
    if(!debug){\
        if(loglevel == LOG_DEBUG || loglevel == LOG_INFO || loglevel == LOG_NOTICE){\
            break;\
        }\
    }\
    if(!foreground){\
        syslog(LOG_FACILITY | loglevel, __VA_ARGS__);\
    }\
    else{\
        logprintf(loglevel, __VA_ARGS__);\
    }\
} while(0);

#endif
