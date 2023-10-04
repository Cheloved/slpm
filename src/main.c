#include <stdio.h>
#include "libs/core.h"

// Just testing
int main(int argc, char** argv)
{
    char* file = (char*)"ls";
    char* arg1  = (char*)"-l";
    char* arg2  = (char*)"-a";
    CMD(file, arg1, arg2, (char*)NULL);
    return 0;
}
