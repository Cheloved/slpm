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

/*
 * Copies using exec()
*/
// cp from target_dir
int Copy_file_to_dir(char* from, char* target_dir);

// cp file1 file2... target_dir
int Copy_files_to_dir(uint32_t argc, char** from_files, char* target_dir);

// cp -r from_dir target_dir
int Copy_dir_to_dir(char* from_dir, char* target_dir);

// cp from to 
int Copy_file_to_file(char* from, char* to);

#endif
