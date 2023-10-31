#ifndef __H_GENTOO_UTILS
#define __H_GENTOO_UTILS

#include <stdio.h>     // Input/output, remove()
#include <stdlib.h>    // exit()
#include <unistd.h>    // UNIX API
#include <stdint.h>    // uint32_t
#include <pthread.h>   // Threads
#include <stdatomic.h> // atomic_int
#include <string.h>
#include <curl/curl.h>

#include "core.h"
#include "curlfetch.h"
#include "defines.h"
#include "packages.h"
#include "gentoo_parser.h"
#include "logging.h"

// This structure is passed
// to threads' function as argument
typedef struct {
    uint32_t id;
    uint32_t thr_count;

    char* search;
    size_t search_len;

    size_t c_count;
    char** categories;
} s_fetch_data;

// Thread function called from search_ebuild()
void* thread_search_ebuild(void *vargp);

// Searches for packages in cloned git repo
int search_ebuild(char* search, uint32_t thr_count);

// Clones git ebuild repo
int fetch_ebuild(char* mirror);

#endif
