#ifndef __H_LOGGING
#define __H_LOGGING

#include <stdio.h>     // Input/output, remove()
#include <stdlib.h>    // exit()
#include <time.h>
#include <stdarg.h>   // For variadic functions

#include "defines.h"

// Enum containing logging levels
enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

// Defines for different logging levels
#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

// Initialize settings 
void log_init(int level, int quiet);

// Main logging function
void log_log(int level, const char *file, int line, const char *fmt, ...);


#endif
