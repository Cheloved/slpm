#include "libs/curlfetch.h"

size_t get_line_len(char* line)
{
    // Iterate over line until end of file or newline symbol
    size_t len = 0;
    while ( line[len] != EOF && line[len] != '\n' )
        len++;

    return len;
}

size_t get_name(char* line, size_t line_len, char** name)
{
    // Find indexes of quotes,
    // between whose name is contained
    char* quote1 = strstr(line, "\'");
    if ( quote1 == NULL )
        return 0;

    char* quote2 = strstr(quote1+1, "\'");
    if ( quote2 == NULL )
        return 0;
    
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
        return 0;
    }

    // Consider line is containing dir 
    // if it starts with
    const char* p_line_start = "<a href=\'";

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

        // Skip if it's ../
        if ( !strcmp(buffer[dir_count], "plain/") )
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

int parse_ebuild_manifest(char* manifest, size_t ver_count, s_package** pkg)
{
    char* spaces[4];
    // Iterate over lines(versions)
    for ( int i = 0; i < ver_count; i++ )
    {
        // Get indexes of spaces to split data from e.g.
        // DIST ansible-7.7.0.tar.gz 40709642 BLAKE2B ee2f8d124f7
        spaces[0] = strstr(manifest+1, " ");
        for ( int j = 1; j < 4; j++ )
            spaces[j] = strstr(spaces[j-1] + 1, " ");

        // Calculate sizes
        size_t archive_len = spaces[1] - spaces[0] - 1;
        size_t size_len = spaces[2] - spaces[1] - 1;

        // Allocate memory
        (*pkg)[i].archive = (char*)calloc(archive_len+1, sizeof(char));
        (*pkg)[i].size_str = (char*)calloc(size_len+1, sizeof(char));

        /* // Copy data */
        memcpy((*pkg)[i].archive, spaces[0]+1, archive_len);
        memcpy((*pkg)[i].size_str, spaces[1]+1, size_len);
        memcpy((*pkg)[i].folder, spaces[3]+1, 2); // First 2 symbols of hash are folder in repo
        (*pkg)[i].folder[2] = '\0';

        // Goto next line
        manifest = strstr(manifest+1, "DIST");
        if ( manifest == NULL )
            break;
    }

    return 0;
}

size_t parse_ebuild_package(char* addr, char* name, s_package** pkg)
{
    // Calculate name length, will be used later
    size_t name_len = strlen(name);

    // Concat manifest path and try to download
    char man_path[MAX_PATH_LEN] = {0};
    strcat(man_path, addr);
    strcat(man_path, "Manifest");

    char* manifest;
    int man_ret_val = download_page(man_path, &manifest);
    if ( man_ret_val != 0 ) // Skip if not exists
        return 0;

    // Get list of files
    char** p_files;
    size_t f_count = get_files(addr, &p_files, 0);

    // Buffer for package' versions
    /* s_package buffer[MAX_VERSIONS]; */
    s_package* buffer = (s_package*)calloc(MAX_VERSIONS, sizeof(s_package));
    size_t ver_count = 0;

    // Iterate over list and find
    // all available versions
    for ( int i = 0; i < f_count; i++ )
    {
        // If file ends with .ebuild,
        // we can extract version, e.g. name-1.2.3.ebuild
        char* ebuild_idx = strstr(p_files[i], ".ebuild");
        if ( ebuild_idx == NULL )
            continue;

        // Calculate length of version and copy
        size_t ver_len = (ebuild_idx - p_files[i]) - name_len;
        buffer[ver_count].ver = (char*)calloc(ver_len+1, sizeof(char));
        memcpy(buffer[ver_count].ver, p_files[i]+name_len, ver_len);

        // Copy name as well
        buffer[ver_count].name = (char*)calloc(name_len+1, sizeof(char));
        memcpy(buffer[ver_count].name, name, name_len-1); // Excluding last symbol (/)

        ver_count++;
    }
    
    // Parse data from manifest
    parse_ebuild_manifest(manifest, ver_count, &buffer);

    // Copy pointer
    *pkg = buffer;

    free(manifest);

    return ver_count;

}
