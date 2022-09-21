#ifndef LIBTMJ_LOG
#define LIBTMJ_LOG

#include <stdbool.h>
#include <stdio.h>

#include "../include/tmj.h"

/**
 * @file
 *
 * @defgroup logging Logging
 *
 * Private logging API.
 */

/**
 * @ingroup logging
 * Statically-allocated memory to hold log messages which are passed to the
 * logging callback.
 */
extern char logmsg_buf[];

/**
 * @ingroup logging
 * Processes log messages and passes them to the active logging callback, if
 * there is one.
 *
 * @param priority One of the set of log priorities defined in the LOG_PRIORITY enum.
 * @param msg      A printf-style format string for the message to be logged
 * @param ...      Format string arguments for the previous argument. See printf() for detail.
 */
void logmsg(log_priority priority, char* msg, ...);

///**
// * @ingroup logging
// * Processes log messages and passes them to the active logging callback, if
// * there is one. This function should not be used directly under normal
// * circumstances. Use the logmsg() macro instead, which uses the preprocessor
// * to add line/file/function debugging information.
// *
// * @param loglevel One of the set of log priorities defined in the LOG_PRIORITY enum.
// * @param line A source-code line number to report as part of the log message.
// * @param file A source-code filename to report as part of the log message.
// * @param func A function name to report as part of the log message.
// * @param msg      A printf-style format string for the message to be logged
// * @param ...      Format string arguments for the previous argument. See printf() for detail.
// */
//void log_event(int loglevel, int line, char* file, char* func, char* msg, ...);


///**
// * @ingroup logging
// * This macro adds line, file, and function name information to logging calls
// * to aid in debugging. Use this macro wherever a message needs to be logged.
// *
// * The expected arguments are identical to that of log_event(). See the
// * documentation for log_event() for detail.
// */
//#define logmsg(loglevel, ...) log_event(loglevel, __LINE__, __FILE__, __func__, msg, __VA_ARGS__);

#endif
