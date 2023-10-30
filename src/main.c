#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/core.h"
#include "libs/defines.h"
#include "libs/utils.h"
#include "libs/webutils.h"
#include "libs/curlfetch.h"

// Just testing
int main(int argc, char** argv)
{
    /* if ( argc == 2 ) */
    /*     if ( !strcmp(argv[1], "fetch") ) */
    /*         fetch_ebuild("https://github.com/gentoo/gentoo"); */

    if ( argc >= 3 )
    {
        if ( !strcmp(argv[1], "search") )
            search_ebuild(argv[2], 10);
    }
    return 0;
}
