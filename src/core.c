#include "libs/core.h"

int is_dir(const char *path)
{
    struct stat statbuf;

    if (stat(path, &statbuf) != 0)
       return 0;

    return S_ISDIR(statbuf.st_mode);
}

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

int Remove_file(char *file)
{
    // Remove file if it exists
    if (access(file, F_OK) != 0)
        return -1;

    remove(file);

    return 0;
}

int Remove_dir(char *dir)
{
    // Try to open to check
    // whether it exists
    DIR* d = opendir(dir);
    if ( errno == ENOENT )
        return 0;

    // Delete if empty
    int ret_val = rmdir(dir);

    // If not, call rm -r dir
    if ( ret_val != 0 && ( errno == ENOTEMPTY || errno == EEXIST ) )
        execlp("rm", "rm", "-r", dir, NULL);

    return 0;
}

int Remove_vec(uint32_t argc, char** files)
{
    for ( int i = 0; i < argc; i++ )
    {
        char* file = files[i];

        if ( is_dir(file) )
            Remove_dir(file);
        else
            Remove_file(file);
    }

    return 0;
}

int Remove_list(uint32_t argc, ...)
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
    int ret_val = Remove_vec(argc, args);

    return ret_val;
}

int Copy_file_to_dir(char* from, char* target_dir)
{
    char* args[2] = { from, target_dir };
    int ret_val = CMD_vec("cp", NULL, 2, args);

    return ret_val;
}

int Copy_files_to_dir(uint32_t argc, char** from_files, char* target_dir)
{
    // Total amount of arguments for cp
    char* args[argc+1];

    for ( int i = 0; i < argc; i++ )
        args[i] = from_files[i];

    args[argc] = target_dir;

    int ret_val = CMD_vec("cp", NULL, argc+1, args);

    return ret_val; 
}

int Copy_dir_to_dir(char* from_dir, char* target_dir)
{
    char* args[3] = { "-r", from_dir, target_dir };
    int ret_val = CMD_vec("cp", NULL, 3, args);

    return ret_val;
}

int Copy_file_to_file(char* from, char* to)
{
    char* args[2] = { from, to };
    int ret_val = CMD_vec("cp", NULL, 2, args);

    return ret_val;
}

size_t Get_files_list(char* source, char*** result, int is_dir)
{
    // Open directory and create
    // entiry buffer
    DIR* d = opendir(source);
    struct dirent* dir;

    // Check if source does not exist
    if ( d == NULL )
        return 0;

    char* buffer[MAX_DIRS];
    size_t count = 0;

    // Iterate over files
    while ((dir = readdir(d)) != NULL)
    {
        // Skip useless
        if ( !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
            continue;

        // If searching for files, but name is directory, skip
        if ( is_dir == 0 && (int)dir->d_type == 4 )
            continue;

        // If searching for directories, but name is file, skip
        if ( is_dir == 1 && (int)dir->d_type == 8 )
            continue;

        // Copy name to buffer
        size_t name_len = strlen(dir->d_name);
        buffer[count] = calloc(name_len+1, sizeof(char));
        memcpy(buffer[count], dir->d_name, name_len);
        count++;
    }

    *result = (char**)calloc(count, sizeof(char*));
    memcpy(*result, buffer, count*sizeof(char*));

    closedir(d);
    return count;
}

size_t Read_file(char* source, char** result)
{
    // Open file
    FILE* f = fopen(source, "r");

    // Check for errors
    if ( f == NULL )
        return 0;

    // Get length of file
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Allocate memory and read
    *result = (char*)calloc(len, sizeof(char));
    fread(*result, sizeof(char), len, f);

    return len;
}
