#include "libs/packages.h"
#include <stdio.h>
#include <stdlib.h>

char* package_to_str(s_package* package)
{
    if ( package->name == NULL || package->ver == NULL ||
         package->size_str == NULL || package->archive == NULL)
        return NULL;

    // Template for package info
    // "name version size archive folder\n"
    char* template = "%s %s %s %s %s\n";

    // Size is length of info's
    // + 3 for folder(extracted from hash)
    // + 4 for spaces
    // + 1 for new line
    // + 1 for null-terminator
    // = 8
    size_t data_size = strlen(package->name) +
                       strlen(package->ver) +
                       strlen(package->size_str) +
                       strlen(package->archive) + 9;

    // Allocate memory
    char* result = (char*)calloc(data_size, sizeof(char));

    // Insert data in template
    sprintf(result, template, package->name, 
                              package->size_str,
                              package->ver,
                              package->archive,
                              package->folder);

    return result;
}

char* package_to_search_result(s_package* package)
{
    if ( package->name == NULL || package->ver == NULL)
        return NULL;

    // Template for package info
    // "name version size archive folder\n"
    char* template = "%s %s\n";

    // Size is length of info's
    // + 1 for spaces
    // + 1 for new line
    // + 1 for null-terminator
    // = 3
    size_t data_size = strlen(package->name) +
                       strlen(package->ver) +
                       3;

    // Allocate memory
    char* result = (char*)calloc(data_size, sizeof(char));

    // Insert data in template
    sprintf(result, template, package->name, 
                              package->ver);

    return result;
}
