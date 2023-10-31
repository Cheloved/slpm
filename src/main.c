#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/core.h"
#include "libs/defines.h"
#include "libs/sys_utils.h"
#include "libs/gentoo_utils.h"
#include "libs/curlfetch.h"
#include "libs/logging.h"

// Just testing
int main(int argc, char** argv)
{
    // Initialize logging
    log_init(LOG_INFO, 0);
    log_init(LOG_TRACE, 0);
    log_info("Program started");

    if ( argc == 2 )
        if ( !strcmp(argv[1], "fetch") )
            fetch_ebuild("https://github.com/gentoo/gentoo");

    if ( argc >= 3 )
    {
        if ( !strcmp(argv[1], "search") )
            search_ebuild(argv[2], 10);
    }
    return 0;
}
