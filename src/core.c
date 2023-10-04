#include "libs/core.h"

int CMD(char *file, ...)
{
    // Maximum quantity of arguments
    // in exec* functions
    const long int ARG_MAX = sysconf(_SC_ARG_MAX);

    // Initialize variadic list
    va_list ap;
    va_start(ap, file);

    // Concatenated argument list
    char* args = (char*)malloc(ARG_MAX);
    bzero(args, ARG_MAX);
    strcat(args, file);

    // Iterate over arguments
    int n = 1;
    char* arg;
    for ( int i = 0; i < ARG_MAX; i++ )
    {
        // Get next one
        arg = va_arg(ap, char*);

        // Check if it is the last one
        if ( arg == NULL )
            break;

        // If not, append to result
        strcat(args, " ");
        strcat(args, arg);
        n++;
    }
    va_end(ap);

    // Split into args_spl by spaces
    // to pass into execvp()
    char* args_spl[n+1];                // Reserve memory of n+1 strings(last is NULL)
    args_spl[0] = strtok(args, " ");    // Get first element

    // Parse the rest of the string
    // (by passing NULL as 1st argument of strtok() we continue splitting the same string)
    for ( int i = 1; i < n; i++ )
        args_spl[i] = strtok(NULL, " ");

    // Add NULL-terminator
    args_spl[n] = (char*)NULL;

    // Execute, passing list of arguments
    // and using PATH variable to access file
    int ret_val = execvp(file, args_spl);

    // Handle errors
    if ( ret_val != 0 )
        fprintf(stderr, " [Error] while executing %s %s.\n%s",
                file, args, strerror(errno));

    // Free memory
    free(arg);
    free(args);
    for ( int i = 0; i < n+1; i++ )
        free(args_spl[i]);

    return ret_val;
}
