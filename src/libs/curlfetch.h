#ifndef __H_CURLFETCH
#define __H_CURLFETCH

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Struct that holds response
typedef struct {
    char* ptr;
    size_t len;
} s_response_str;

// Initializes response struct
void init_string(s_response_str* s);

// Adds new block of data to response
size_t write_callback(void* ptr, size_t size,
                      size_t nmemb, s_response_str* s);

// Downloads page and writes result to content 
// WARNING: you should free content after using
int download_page(char* path, char** content);

#endif
