#include "libs/webparser.h"
#include "libs/curlfetch.h"
#include "libs/packages.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t get_line_len(char* line)
{
    // Iterate over line until end of file or newline symbol
    size_t len = 0;
    while ( line[len] != EOF && line[len] != '\n' )
        len++;

    return len;
}

int parse_gentoo_line(char* line, size_t len, s_package* package)
{
    /* printf(" [DEBUG] Line: "); */
    /* for ( int i = 0; i < len; i++ ) */
    /*     printf("%c", line[i]); */
    /* printf("\n"); */

    // Parse filename from line
    uint32_t fname_start = -1; // Start index
    uint32_t fname_end   = -1; // End index

    // Buffer for filename
    char filename[512];
    bzero(filename, 512);
    size_t filename_len = 0;

    // Iterate over line
    for ( int i = 0; i < len; i++ )
    {
        // Since line with file data should look like this:
        // <a href="x11-clipboard-0.5.3.crate">x11-clipboard-0.5.3.crate</a> 23-Oct-2021 04:36    7789
        // exctracting filename of archive between double quotes
        if ( line[i] == '\"' )
        {
            if ( fname_start == -1 ) { fname_start = i+1; continue; }
            else { fname_end = i-1; break; }
        } 

        // If line[i] is between double quotes
        // write to filename
        if ( fname_start != -1 )
            filename[filename_len++] = line[i];
    }

    // Handle useless lines
    if ( filename_len == 0 || !strcmp(filename, "../") )
        return -1;

    // Copy name of archive to struct
    package->name = (char*)calloc(filename_len+1, sizeof(char)); 
    memcpy(package->name, filename, filename_len);

    /* printf(" [DEBUG] Archive name recieved: %s\n", filename); */
    /* printf(" [DEBUG] Archive name recieved: %s\n", package->name); */

    return 0;
}

size_t parse_gentoo_dir(char* addr, size_t addr_size,
                        char* content, s_package** packages)
{
    // Get length of page content
    uint32_t content_len = strlen(content);

    // Consider line is containing package data
    // if it starts with
    const char* p_line_start = "<a href=\"";
    size_t start_len = strlen(p_line_start);

    // Define array of packages
    s_package buffer[MAX_PACK_PER_GENTOO_DIR];
    size_t p_count = 0; // Amount of packages

    // Indicates that current symbol
    // is the first one on the line
    int linestart  = 1;
    for ( int i = 0; i < content_len; i++)
    {
        // If line contains package data
        if ( linestart && !strncmp(content+i, p_line_start, start_len) ) 
        {
            size_t line_len = get_line_len(content+i);
            int ret_val = parse_gentoo_line(content+i, line_len, &buffer[p_count]);

            // If parsed successfully,
            // write address of file to struct
            if ( ret_val == 0 )
            {
                // Allocate memory for string "addr/package_name"
                buffer[p_count].link = (char*)calloc(addr_size+strlen(buffer[p_count].name), sizeof(char));

                // Write full file path in it
                strcat(buffer[p_count].link, addr);
                strcat(buffer[p_count].link, buffer[p_count].name);

                // Increase amount of packages
                p_count++;
            }
            
            linestart = 0;
        }

        if ( content[i] == '\n' )
            linestart = 1;
    }

    *packages = (s_package*)calloc(p_count, sizeof(s_package));
    memcpy(*packages, buffer, p_count*sizeof(s_package));

    return p_count;
}

size_t get_name(char* line, size_t line_len, char** name)
{
    // Find indexes of quotes,
    // between whose name is contained
    char* quote1 = strstr(line, "\'");
    if ( quote1 == NULL )
        return -1;

    char* quote2 = strstr(quote1+1, "\'");
    if ( quote2 == NULL )
        return -1;
    
    // Find previous slash
    char* p = quote2 - 2;
    while ( *p != '/' )
        p--;

    // Calculate length of name
    size_t name_len = quote2-1 - p;

    // Copy data
    *name = (char*)calloc(name_len+1, sizeof(char));
    memcpy(*name, p+1, name_len);

    return name_len;
}

size_t get_files(char* addr, char*** dirs, int is_dir)
{
    // Download page
    char* content;
    int ret_val = download_page(addr, &content);
    if ( ret_val != 0 )
    {
        fprintf(stderr, " [ERROR] Can't download page %s\n", addr);
        return -1;
    }

    // Get length of page content
    uint32_t content_len = strlen(content);

    // Consider line is containing dir 
    // if it starts with
    const char* p_line_start = "<a href=\'";
    size_t start_len = strlen(p_line_start);

    // Define buffer array of dirs 
    char* buffer[MAX_DIRS];
    size_t dir_count = 0; // Amount of dirs 

    char* tmp = content; // Temporary pointer to content
    char* start;         // Pointer to the start of a line

    // Read while there are still occurences of p_line_start
    while ( (start = strstr(tmp, p_line_start)) != NULL )
    {
        // Get length of a line
        size_t line_len = get_line_len(start);

        // Parse name
        size_t name_len = get_name(start, line_len, &buffer[dir_count]);

        // Move tmp pointer further
        tmp = start+1;

        // Handle wrong lines
        if ( name_len < 0 || buffer[dir_count] == NULL )
        {
            free(buffer[dir_count]);
            continue;
        }

        // If searching for files, but name is directory, skip
        if ( is_dir == 0 && buffer[dir_count][name_len-1] == '/' )
        {
            free(buffer[dir_count]);
            continue;
        }

        // If searching for directories, but name is file, skip
        if ( is_dir == 1 && buffer[dir_count][name_len-1] != '/' )
        {
            free(buffer[dir_count]);
            continue;
        }

        /* printf(" [DEBUG] Dir found: \"%s\"\n", buffer[dir_count]); */

        // Increease amount
        dir_count++;
    }

    *dirs = (char**)calloc(dir_count, sizeof(char*));
    memcpy(*dirs, buffer, dir_count*sizeof(char*));

    return dir_count;
}
