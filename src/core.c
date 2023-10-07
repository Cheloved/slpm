#include "libs/core.h"
#include <unistd.h>

int CMD_vec(char* file, int* fd, uint32_t argc, char** argv)
{
    // Total amount of arguments,
    // including file itself and NULL-terminator
    uint32_t targc = argc+2; 

    // Vector of all arguments
    char* args[targc];

    // Copy pointers
    for ( int i = 0; i < targc; i++ )
        args[i+1] = argv[i];

    // Assign first and last elements
    // according to execvp() requirements
    args[0]       = file;
    args[targc-1] = (char*)NULL;

    // Define pipe file descriptors
    // and create a pipe
    int pipefd[2]; // pipefd[0] - read end, pipefd[1] - write end
    pipe(pipefd);

    // Return read file descriptor in fd
    // if needed
    if ( fd != NULL )
        *fd = pipefd[0];

    // Fork process
    pid_t pid = fork();
    if ( pid < 0 )
    {
        fprintf(stderr, " [Error] while executing %s", file);
        for ( int i = 1; i < targc-1; i++ )
            fprintf(stderr, " %s", args[i]);
        fprintf(stderr, ":\n  Unable to fork()");
    }

    if ( pid == 0 )
    {
        // Close reading end in the child
        close(pipefd[0]);

        // Send stdout and stderr to the pipe
        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);

        // As child process execute command, passing list of arguments
        // and using PATH variable to access the file
        int ret_val = execvp(file, args);

        // Since this descriptor is no longer needed,
        // close it
        close(pipefd[1]);

        exit(ret_val);
    } else {
        // Close the write end of the pipe in the parent
        close(pipefd[1]);

        // As parent wait for child process to end
        int status = -1; // Return status of child process
        if ( waitpid(pid, &status, 0) == -1 )
        {
            fprintf(stderr, " [Error] in CMD_vec(). waitpid() failed\n");
            return -1;
        }

        // Handle child's exit status
        if ( WIFEXITED(status) )
        {
            int es = WEXITSTATUS(status);

            // Handle errors
            if ( es != 0 )
            {
                fprintf(stderr, " [Error] while executing %s", file);
                for ( int i = 1; i < targc-1; i++ )
                    fprintf(stderr, " %s", args[i]);
                fprintf(stderr, "\n");
                /* fprintf(stderr, ":\n  %s", strerror(errno)); */
            }

            return es;
        }
    }

    return 0;
}

int CMD_list(char* file, int* fd, uint32_t argc, ...)
{
    // Initialize variadic list
    va_list ap;
    va_start(ap, argc);

    // Vector of all arguments
    char* args[argc];

    // Copy pointers
    for ( int i = 0; i < argc; i++ )
        args[i] = va_arg(ap, char*);
    va_end(ap);

    // Call 
    int ret_val = CMD_vec(file, fd, argc, args);

    return ret_val;
}
