#ifndef __H_CORE
#define __H_CORE

#include <stdio.h>    // Input/output, remove()
#include <stdlib.h>   // exit()
#include <unistd.h>   // UNIX API
#include <sys/wait.h> // waitpid()
#include <stdint.h>   // uint32_t
#include <errno.h>    // For handling errors
#include <stdarg.h>   // For variadic functions
#include <sys/stat.h> // stat(), S_ISDIR

// Check if path is directory
int is_dir(const char* path);

/*
 * Executes "file" command with give list of argc arguments.
 *
 * If output of command need to be parsed,
 * file descriptor to read from will be returned as int* fd.
 * If note, NULL may be passed
*/
int CMD_vec(char* file, int* fd, uint32_t argc, char** argv);
int CMD_list(char* file, int* fd, uint32_t argc, ...);

/*
 * Calls Remove_file() or Remove_dir(),
 * depending on entity type
*/
int Remove_vec(uint32_t argc, char** files);
int Remove_list(uint32_t argc, ...);

int Remove_file(char* file);
int Remove_dir(char* dir);


#endif
