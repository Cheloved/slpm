#include "libs/curlfetch.h"

void init_string(s_response_str* s)
{
    s->len = 0;
    s->ptr = calloc(s->len + 1, sizeof(char));
    s->ptr[0] = '\0';
}

size_t write_callback(void* ptr, size_t size, 
                      size_t nmemb, s_response_str* s)
{
    const size_t add_len = size*nmemb;
    const size_t new_len = s->len + add_len;

    // Create extended char array
    char* newptr = calloc(new_len + 1, sizeof(char));

    // Copy old data
    memcpy(newptr, s->ptr, s->len);

    // Copy new data
    memcpy(newptr + s->len, (char*)ptr, add_len);

    // Reassign
    free(s->ptr);
    s->ptr = newptr;
    s->len = new_len;

    return size*nmemb;
}

int download_page(char* path, char** content)
{
    // Client and variable for return code
    CURL* curl;
    CURLcode result;

    // Initialize client
    curl = curl_easy_init();
    if ( !curl )
    {
        perror(" [E] Error initializing curl");
        return 1;
    }

    // Initialize structure for holding response data
    s_response_str response;
    init_string(&response);

    // Set options
    curl_easy_setopt(curl, CURLOPT_URL, path);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    /* curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L); */

    // GET request
    result = curl_easy_perform(curl);
    if ( result != CURLE_OK || result == CURLE_HTTP_RETURNED_ERROR )
        return -1;

    // Return content
    *content = response.ptr;

    // Clean things up
    curl_easy_cleanup(curl);

    return 0;
}
