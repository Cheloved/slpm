#include "libs/webutils.h"
#include "libs/core.h"
#include "libs/packages.h"
#include "libs/webparser.h"
#include <stdio.h>
#include <strings.h>

/*
 * This variable locks access to 
 * file so that only one thread has
 * access to it
*/
atomic_int lock;
 
/*
 * This function is executed by threads.
 * Depending on thread ID it parses
 * the corresponding folders in Gentoo mirror
*/
void* thread_fetch_gentoo(void *vargp) 
{ 
    // Store the ID value
    s_fetch_data* data = (s_fetch_data*) vargp;
    /* printf(" [DEBUG] Thread ID: %u\n", data->id); */ 

    // Get folder names to fetch
    // by getting start and end indexes
    // and converting them to hex values later on
    uint32_t dir_per_thread = (uint32_t)255 / data->thr_count;
    uint32_t start_dir = data->id * dir_per_thread;
    uint32_t end_dir = 0;
    if ( data->id < data->thr_count - 1 )
        end_dir = (data->id+1) * dir_per_thread - 1;
    else
        end_dir = 255;

    printf(" [DEBUG] Thread %u\n\tStart: %u\n\tEnd: %u\n\tDPT: %u\n\n",
            data->id, start_dir, end_dir, dir_per_thread);

    // Allocate memory to be enough even
    // if amount of threads is 1.
    // 2MB in this case
    char* packages_string = (char*)calloc(THR_BUFFER_SIZE, sizeof(char));
    uint32_t length = 0;

    for ( uint32_t i = start_dir; i < end_dir; i++ )
    {
        // Template will turn in smth like
        // https://mirror.domain/f3/
        char* template = i < 16? "%s/distfiles/0%x/" : "%s/distfiles/%x/";

        // Size of address is length of mirror
        // + 9 for distfiles
        // + 4 for four slashes
        // + 2 for index of folder
        // + 1 for null-terminator
        // = 16
        size_t addr_size = strlen(data->mirror) + 16;
        char addr[addr_size];
        bzero(addr, addr_size);

        // Insert data in template
        sprintf(addr, template, data->mirror, i);

        // Download page using libcurl
        char* content;
        download_page(addr, &content);

        // Parse all packages
        s_package* packages;
        size_t p_count = parse_gentoo_dir(addr, addr_size, content, &packages);

        // Buffer for conversion from package to string
        char* p_to_s_buffer;
        for ( int n = 0; n < p_count; n++ )
        {
            p_to_s_buffer = package_to_str(&packages[n]);
            strcat(packages_string, p_to_s_buffer);
            free(p_to_s_buffer);
        }
    }

    // If file is locked,
    // wait 1ms
    while ( lock )
        usleep(1000);

    // When file is unlocked,
    // lock it and write data to it
    lock = 1;
    printf(" [DEBUG] Thread %u locked file\n", data->id); 

    FILE *fptr;

    // Open a file in append mode
    fptr = fopen("parsed.txt", "a");

    // Write some text to the file
    fprintf(fptr, "%s", packages_string);

    // Close the file
    fclose(fptr);

    lock = 0;
    printf(" [DEBUG] Thread %u unlocked file\n", data->id); 

    printf(" [DEBUG] Thread %d ended\n", data->id);
    pthread_exit(0);
} 

int fetch_gentoo(uint32_t thr_count, char* mirror)
{
    // Unlock file
    lock = 0;
  
    // Array that hold thread IDs
    pthread_t thread_ids[thr_count];

    // Data structs will be passed as arguments
    s_fetch_data data[thr_count];
    for ( int i = 0; i < thr_count; i++ )
    {
        thread_ids[i] = 0;
        data[i].id = i;
        data[i].thr_count = thr_count;
        data[i].mirror = mirror;
    }

    // Create threads and call thread_fetch_gentoo()
    for ( int i = 0; i < thr_count; i++ )
        pthread_create(&thread_ids[i], NULL, 
                       thread_fetch_gentoo, (void *)&data[i]); 

    // Join threads
    for( int i = 0; i < thr_count; i++)
        pthread_join(thread_ids[i], NULL);
  
    return 0; 
}