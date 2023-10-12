#include <stdio.h>
#include <stdlib.h>
#include "libs/core.h"
#include "libs/utils.h"
#include "libs/webutils.h"
#include "libs/curlfetch.h"

// Just testing
int main(int argc, char** argv)
{
    /* check_root(); */
    fetch_gentoo(10, "https://mirror.yandex.ru/gentoo-distfiles");
    /* char* content; */
    /* download_page("https://mirror.yandex.ru/gentoo-distfiles/distfiles/00/", &content); */
    /* printf("%s", content); */

    return 0;
}
