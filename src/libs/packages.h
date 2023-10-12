#ifndef __H_PACKAGES
#define __H_PACKAGES

#include <stdint.h>

// Structure that holds 
// basic information about package
typedef struct
{
    char* name;
    char* ver;
    char* last_upload;
    uint32_t size;
    char* link;
} s_package;

#endif
