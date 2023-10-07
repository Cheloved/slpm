#include <stdio.h>
#include "libs/core.h"

// Just testing
int main(int argc, char** argv)
{
    /* char* file = (char*)"lss"; */
    /* char* arg1  = (char*)"-l"; */
    /* char* arg2  = (char*)"-a"; */
    /* for ( int i = 0; i < 50; i++ ) */
    /* { */
    /*     CMD_list(file, NULL, 2, arg1, arg2); */
    /*     CMD_list(file, NULL, 3, "-l", "-a", "-h"); */
    /* } */

    char* file = (char*)"ls";
    char* arg1  = (char*)"-l";
    char* arg2  = (char*)"-a";
    int fd = 0;
    CMD_list(file, &fd, 2, arg1, arg2);

    char buffer[1024];
    while (read(fd, buffer, sizeof(buffer)) != 0)
        printf("READED FROM FD:\n%s", buffer);
    printf("READ END\n");



    /* char* file2 = (char*)"grep"; */
    /* char* args[2] = {"return", "src/main.c"}; */
    /* CMD_vec(file2, 2, args); */
    return 0;
}
