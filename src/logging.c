#include "libs/logging.h"

// Settings of logger
static struct
{
    int quiet;
    int level;
} LOG_CONFIG = {0, LOG_INFO};

// Names of log levels to output
const char* LEVEL_NAMES[6] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

void log_init(int level, int quiet)
{
    if ( level < 0 || level > 5 )
    {
        log_error("Incorrect log level. Set to info as default");
        LOG_CONFIG.level = LOG_INFO;
    }

    LOG_CONFIG.level = level;
    LOG_CONFIG.quiet = quiet;
}

void log_log(int level, const char *file, int line, const char *fmt, ...)
{
    if ( LOG_CONFIG.quiet || level < LOG_CONFIG.level )
        return;

    // Print info about where function was called
    if ( level == LOG_INFO )
        fprintf(stdout, " [%s] ", LEVEL_NAMES[level]);
    else
        fprintf(stdout, " [%s] in (%s) at line %d: ", LEVEL_NAMES[level], file, line);
    
    // Print other info
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);

    // Add new line
    fprintf(stdout, "\n");
}
