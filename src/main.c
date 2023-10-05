#include <stdio.h>
#include "libs/core.h"

// Just testing
int main(int argc, char** argv)
{
    char* file = (char*)"ls";
    char* arg1  = (char*)"-l";
    char* arg2  = (char*)"-a";
    for ( int i = 0; i < 50; i++ )
    {
        /* CMD_list(file, 2, arg1, arg2); */
        CMD_list(file, 1, arg2);
    }

    char* file2 = (char*)"grep";
    char* args[2] = {"return", "src/main.c"};
    CMD_vec(file2, 2, args);
    return 0;
}
