#ifndef __H_CORE
#define __H_CORE

#include <stdio.h>    // Input/output
#include <stdlib.h>   // malloc()
#include <unistd.h>   // UNIX API
#include <sys/wait.h> // waitpid()
#include <stdint.h>   // uint32_t

#include <errno.h>    // For handling errors
#include <string.h>
#include <strings.h>  // bzero()

#include <stdarg.h>   // For variadic functions

/*
 * Executes "file" command with give list of argc arguments
*/
int CMD_vec(char* file, uint32_t argc, char** argv);
int CMD_list(char* file, uint32_t argc, ...);

#endif
