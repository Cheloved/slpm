#include "libs/packages.h"

char* package_to_str(s_package* package)
{
    // Allocate memory for name and link(for now)
    // + 4 for null-terminator and newlines
    char* result = (char*)calloc(strlen(package->name) +
                                 strlen(package->link) +
                                 4, sizeof(char));

    strcat(result, package->name);
    strcat(result, "\n");
    strcat(result, package->link);
    strcat(result, "\n\n");

    return result;
}
