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
 * Executes "file" command with give list of argc arguments.
 *
 * If output of command need to be parsed,
 * file descriptor to read from will be returned as int* fd.
 * If note, NULL may be passed
*/
int CMD_vec(char* file, int* fd, uint32_t argc, char** argv);
int CMD_list(char* file, int* fd, uint32_t argc, ...);

#endif
