#ifndef __H_WEBPARSER
#define __H_WEBPARSER

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "packages.h"

#define MAX_PACK_PER_GENTOO_DIR 512
#define MAX_DIRS 2048
#define MAX_PATH_LEN 512

// Gets length of a line
size_t get_line_len(char* line);

// Extracts name of file or folder in given line
size_t get_name(char* line, size_t line_len, char** name);

// Gets list of files or directories addresses
// is_dir = 0  - search for files only
// is_dir = 1  - search for folders only
// is_dir = -1 - search for both
size_t get_files(char* addr, char*** dirs, int is_dir);

// Parse package folder in ebuild repo
// and returns all versions of package
size_t parse_ebuild_package(char* addr, char* name, s_package** pkg);

// Parses content of line into package struct
int parse_gentoo_line(char* line, size_t len, s_package* package);

/*
 * Parses the content of Gentoo repo's dir
 * and extracts info about packages available in it.
 * Returns amount of packages and array of s_package with info.
 * Negative returns indicate error
*/
size_t parse_gentoo_dir(char* addr, size_t addr_size,
                        char* content, s_package** packages);


#endif
