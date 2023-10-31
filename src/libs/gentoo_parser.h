#ifndef __H_WEBPARSER
#define __H_WEBPARSER

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "packages.h"
#include "defines.h"
#include "core.h"
#include "logging.h"

// Parse manifest file
// and extract archive name, size and BLAKE2 hash
int parse_ebuild_manifest(char* manifest, size_t ver_count, s_package** pkg);

// Parse package folder in ebuild repo
// and returns all versions of package
size_t parse_ebuild_package(char* addr, char* name, s_package** pkg);

#endif
