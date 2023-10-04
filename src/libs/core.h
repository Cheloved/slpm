#ifndef __H_CORE
#define __H_CORE

#include <stdio.h>   // Input/output
#include <stdlib.h>  // malloc()
#include <unistd.h>  // UNIX API

#include <errno.h>   // For handling errors
#include <string.h>
#include <strings.h> // bzero()

#include <stdarg.h>  // For variadic functions
#include <limits.h>  // _SC_ARG_MAX

/*
 * Executes "file" command with NULL-terminated
 * list of arguments
*/
int CMD(char* file, ...);

#endif
