#ifndef __H_WEBUTILS
#define __H_WEBUTILS

#include <stdio.h>     // Input/output, remove()
#include <stdlib.h>    // exit()
#include <unistd.h>    // UNIX API
#include <stdint.h>    // uint32_t
#include <pthread.h>   // Threads
#include <stdatomic.h> // atomic_int
#include <string.h>
#include "core.h"

#define THR_BUFFER_SIZE 2097152
#define THR_BATCH_SIZE 524288

// This structure is passed
// to threads' function as argument
typedef struct {
    uint32_t id;
    uint32_t thr_count;
    char* mirror;
} s_fetch_data;

/* 
 * Gets list of all available packages from Gentoo mirror.
 * Returns list in file descriptor fd.
 * Parses folders using thr_count threads.
*/
void* thread_fetch_gentoo(void *vargp);
int fetch_gentoo(uint32_t thr_count, char* mirror);

#endif
