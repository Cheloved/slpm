#ifndef __H_PACKAGES
#define __H_PACKAGES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Structure that holds 
// basic information about package
typedef struct
{
    char* name;
    char* ver;
    char* last_upload;
    char* size_str;
    uint32_t size;
    char* link;
} s_package;

// Converts struct data to string
char* package_to_str(s_package* package);

#endif
