#ifndef __H_WEBPARSER
#define __H_WEBPARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "packages.h"

#define MAX_PACK_PER_GENTOO_DIR 512

// Gets length of a line
size_t get_line_len(char* line);

// Parses content of line into package struct
int parse_gentoo_line(char* line, size_t len, s_package* package);

/*
 * Parses the content of Gentoo repo's dir
 * and extracts info about packages available in it.
 * Returns amount of packages and array of s_package with info.
 * Negative returns indicate error
*/
size_t parse_gentoo_dir(char* addr, char* content, s_package* packages);


#endif
