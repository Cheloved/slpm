#include "libs/webparser.h"
#include <stdint.h>
#include <string.h>
#include <strings.h>

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
    uint32_t name_start = -1;
    uint32_t name_end   = -1;
    char buffer[512];
    bzero(buffer, 512);
    size_t buffer_len = 0;

    for ( int i = 0; i < len; i++ )
    {
        if ( line[i] == '\"' )
        {
            if ( name_start == -1 ) { name_start = i+1; continue; }
            else { name_end = i-1; break; }
        } 

        if ( name_start != -1 ) {
            buffer[buffer_len++] = line[i];
        }
    }
    /* int name_len = name_start - name_len; */
    /* char name[name_len + 1]; */
    /* bzero(name, name_len+1); */

    /* memcpy(name, line+name_start, name_len); */
    printf(" [DEBUG] Package name recieved: %s\n", buffer);

    return 0;
}

size_t parse_gentoo_dir(char* addr, char* content, s_package* packages)
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
        if ( linestart && strncmp(content+i, p_line_start, start_len) != 0 ) 
        {
            size_t line_len = get_line_len(content+i);
            int ret_val = parse_gentoo_line(content+i, line_len, &buffer[p_count]);
            if ( ret_val == 0 )
                p_count++;
            
            linestart = 0;
        }

        if ( content[i] == '\n' )
            linestart = 1;
    }

    return 0;
}
