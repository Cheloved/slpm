#include "libs/webparser.h"
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
    printf(" [DEBUG] Line: ");
    for ( int i = 0; i < len; i++ )
        printf("%c", line[i]);
    printf("\n");

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

    // Parse version from filename
    uint32_t ver_start = -1; // Start index
    uint32_t ver_end   = -1; // End index

    // Buffers for version
    // Because it reads from end to start,
    // it gets reversed version, so we need to
    // reverse it back
    char rev_ver[512];   // Buffer for reversed version
    bzero(rev_ver, 512);

    char ver[512];       // Buffer for actual version
    bzero(ver, 512);

    size_t ver_len = 0;

    // Iterate over file name from end
    for ( int i = filename_len; i >= 0; i-- )
    {
        // Copy from first dot, followed by digit, to the first dash
        // e.g. x11-clipboard-0.5.3.crate -> 3.5.0
        if ( ver_end == -1 && filename[i] == '.' && isdigit(filename[i-1]) ) 
        {
            ver_end = i-1;
            continue;
        }
        if ( filename[i] == '-' && !isdigit(filename[i-1]) ) { ver_start = i+1; break; }

        if ( ver_end != -1 )
            rev_ver[ver_len++] = filename[i];
    }

    // Reverse version: 3.5.0 -> 0.5.3
    for ( int i = ver_len-1; i >= 0; i-- )
        ver[ver_len-1-i] = rev_ver[i];

    // Parsing the name of the package
    char name[512];
    bzero(name, 512);
    size_t name_len = 0;

    // If package name contains version
    // copy from filename from beginning to ver_start - 2 inclusivly
    if ( ver_len > 0 )
        for ( int i = 0; i < ver_start - 1; i++ )
            name[name_len++] = filename[i];
    else // If not, parse from beginning to first dot
        for ( int i = 0; i < filename_len; i++ )
        {
            if ( filename[i] == '.' )
                break;
            name[name_len++] = filename[i];
        }


    printf(" [DEBUG] Archive name recieved: %s\n", filename);
    printf(" [DEBUG] Package name recieved: %s\n", name);
    printf(" [DEBUG] Package version recieved: %s\n\n", ver);

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
        if ( linestart && !strncmp(content+i, p_line_start, start_len) ) 
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
