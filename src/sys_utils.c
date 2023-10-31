#include "libs/sys_utils.h"

void check_root()
{
    // Get effective user ID.
    // The value of zero means root privileges
    if (geteuid() != 0)
    {
        fprintf(stderr, " [ERROR] SLPM must be run with root privileges\n");
        exit(1);
    }
}
