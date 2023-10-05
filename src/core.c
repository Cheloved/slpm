#include "libs/core.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

int CMD_vec(char* file, uint32_t argc, char** argv)
{
    // Total amount of arguments,
    // including file itself and NULL-terminator
    uint32_t targc = argc+2; 

    // Vector of all arguments
    char* args[targc];

    // Copy pointers
    for ( int i = 0; i < targc; i++ )
        args[i+1] = argv[i];

    // Assign first and last elements
    // according to execvp() requirements
    args[0]       = file;
    args[targc-1] = (char*)NULL;

    // Execute, passing list of arguments
    // and using PATH variable to access the file
    int ret_val = execvp(file, args);

    // Handle errors
    if ( ret_val != 0 )
    {
        fprintf(stderr, " [Error] while executing %s", file);
        for ( int i = 1; i < targc-1; i++ )
            fprintf(stderr, " %s", args[i]);
        fprintf(stderr, ":\n  %s", strerror(errno));
    }

    return ret_val;
}

int CMD_list(char* file, uint32_t argc, ...)
{
    // Initialize variadic list
    va_list ap;
    va_start(ap, argc);

    // Vector of all arguments
    char* args[argc];

    // Copy pointers
    for ( int i = 0; i < argc; i++ )
        args[i] = va_arg(ap, char*);
    va_end(ap);

    // Call 
    int ret_val = CMD_vec(file, argc, args);

    return ret_val;
}
