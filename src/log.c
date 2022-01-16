#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <syslog.h>
#include <time.h>

#define DATE_BUFFER_SIZE 100

bool debug = false;
bool foreground = true;

void logprintf(int loglevel, char* msg, ...){
    va_list args;
    va_start(args, msg);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    char date_buffer[DATE_BUFFER_SIZE];
    struct tm* bd_time = localtime(&ts.tv_sec);
    strftime(date_buffer, DATE_BUFFER_SIZE, "[%F %T]", bd_time);

    if(loglevel == LOG_ALERT){
        printf("%s Alert: ", date_buffer);
    }
    else if(loglevel == LOG_CRIT){
        printf("%s Critical: ", date_buffer);
    }
    else if(loglevel == LOG_DEBUG){
        printf("%s Debug: ", date_buffer);
    }
    else if(loglevel == LOG_EMERG){
        printf("%s Emergency: ", date_buffer);
    }
    else if(loglevel == LOG_ERR){
        printf("%s Error: ", date_buffer);
    }
    else if(loglevel == LOG_INFO){
        printf("%s Info: ", date_buffer);
    }
    else if(loglevel == LOG_NOTICE){
        printf("%s Notice: ", date_buffer);
    }
    else if(loglevel == LOG_WARNING){
        printf("%s Warning: ", date_buffer);
    }

    vprintf(msg, args);
    printf("\n");

    va_end(args);

    fflush(NULL);
}
