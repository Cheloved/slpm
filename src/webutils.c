#include "libs/webutils.h"

void* thread_search_ebuild(void *vargp) 
{ 
    // Store the ID value
    s_fetch_data* data = (s_fetch_data*) vargp;
    log_trace("Thread %u started", data->id); 

    // Get folder names to fetch
    // by getting start and end indexes
    uint32_t dir_per_thread = data->c_count / data->thr_count;
    uint32_t start_dir = data->id * dir_per_thread;
    uint32_t end_dir = 0;
    if ( data->id < data->thr_count - 1 )
        end_dir = (data->id+1) * dir_per_thread - 1;
    else
        end_dir = data->c_count;

    // Allocate memory to be enough even
    // if amount of threads is 1.
    // 2MB in this case
    char* search_results = (char*)calloc(THR_BUFFER_SIZE, sizeof(char));

    // Iterate over categories
    for ( uint32_t i = start_dir; i < end_dir; i++ )
    {
        // Template for full path
        char* template = "%s/%s/";

        // Size of address is length of mirror
        // + length of category
        // + 2 for /
        // + 1 for null-terminator
        size_t addr_size = strlen(GIT_REPO_PATH) +
                           strlen(data->categories[i]) + 3;
        char addr[addr_size];
        bzero(addr, addr_size);

        // Insert data in template
        sprintf(addr, template, GIT_REPO_PATH, data->categories[i]);

        // Parse all packages directories
        char** p_dirs;
        size_t p_count = Get_files_list(addr, &p_dirs, 1);
        if ( p_count == 0 )
            continue;

        log_trace("Thread %d found %lu packages in %s", data->id, p_count, data->categories[i]);

        // Iterate over packages dirs and search
        for ( int j = 0; j < p_count; j++ )
        {
            // Check for search_string
            // If occurrence is found, parse directory
            if ( strstr(p_dirs[j], data->search) == NULL )
                continue;

            // Concatenate addr and package name to get full path
            char p_path[MAX_PATH_LEN] = {0};
            strcat(p_path, addr);
            strcat(p_path, p_dirs[j]);

            // Parse all available versions of package
            s_package* pkgs;
            size_t ver_count = parse_ebuild_package(p_path, p_dirs[j], &pkgs);
            if ( ver_count == 0 )
                continue;

            // Convert packages' structs into strings
            // Output for user one string
            // and write to tmp another
            for ( int n = 0; n < ver_count; n++ )
            {
                char* search_str = package_to_search_result(&pkgs[n]);
                char* buffer = package_to_str(&pkgs[n]);
                if ( search_str != NULL )
                {
                    log_info("%s", search_str);
                    free(search_str);
                }
                if ( buffer != NULL )
                {
                    strcat(search_results, buffer);
                    free(buffer);
                }
            }
        }
    }

    // Open a file in append mode
    FILE *fptr;
    fptr = fopen(SEARCH_TMP_PATH, "a");

    // Write some text to the file
    fprintf(fptr, "%s", search_results);

    // Close the file
    fclose(fptr);

    log_trace("Thread %d finished", data->id);

    pthread_exit(0);
} 

int search_ebuild(char* search, uint32_t thr_count)
{
    Remove_file(SEARCH_TMP_PATH);

    char** categories;
    size_t c_count = Get_files_list(GIT_REPO_PATH, &categories, 1);
    log_debug("%d categories found", c_count);

    if ( c_count == 0 )
    {
        log_error("Repository not found. Try \'slpm fetch\'"); 
        return 0;
    }
  
    // Array that hold thread IDs
    pthread_t thread_ids[thr_count];

    // Data structs will be passed as arguments
    s_fetch_data data[thr_count];

    // Calculate length of search string
    size_t search_len = strlen(search);

    // Create threads and call thread_fetch_gentoo()
    for ( int i = 0; i < thr_count; i++ )
    {
        thread_ids[i] = 0;
        data[i].id = i;
        data[i].thr_count = thr_count;
        data[i].search = search;
        data[i].search_len = search_len;
        data[i].c_count = c_count;
        data[i].categories = categories;
        pthread_create(&thread_ids[i], NULL, 
                       thread_search_ebuild, (void *)&data[i]); 
    }

    // Join threads
    for( int i = 0; i < thr_count; i++)
        pthread_join(thread_ids[i], NULL);

    return 0; 
}

int fetch_ebuild(char* mirror)
{
    // Remove previous clone if exists
    Remove_dir(GIT_REPO_PATH); 

    // Cloning repo
    int retval = CMD_list("git", NULL, 5, "clone", "--depth=1", "-j5", mirror, GIT_REPO_PATH);

    if ( retval == 0 )
        printf(" Successfully fetched\n");
    else
        fprintf(stderr, " Error occurred while fetching.\nCheck internet connection or mirrors\n");
    
    return 0;
}
