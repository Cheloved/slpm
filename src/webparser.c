#include "libs/webparser.h"

size_t get_line_len(char* line)
{
    // Iterate over line until end of file or newline symbol
    size_t len = 0;
    while ( line[len] != EOF && line[len] != '\n' )
        len++;

    return len;
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
    strcat(man_path, "/");
    strcat(man_path, "Manifest");

    // Read Manifest file
    char* manifest;
    size_t man_len = Read_file(man_path, &manifest);

    // Skip if Manifest file does not exist
    if ( man_len == 0 )
        return 0;

    // Get list of files
    char** p_files;
    size_t f_count = Get_files_list(addr, &p_files, 0);

    // Buffer for package' versions
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
        size_t ver_len = (ebuild_idx - p_files[i]) - name_len-1;
        buffer[ver_count].ver = (char*)calloc(ver_len+1, sizeof(char));
        memcpy(buffer[ver_count].ver, p_files[i]+name_len+1, ver_len);

        // Copy name as well
        buffer[ver_count].name = (char*)calloc(name_len+1, sizeof(char));
        memcpy(buffer[ver_count].name, name, name_len);

        ver_count++;
    }
    
    // Parse data from manifest
    parse_ebuild_manifest(manifest, ver_count, &buffer);

    // Copy pointer
    *pkg = buffer;

    free(manifest);

    return ver_count;

}
