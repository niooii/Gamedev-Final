#ifdef COMPILE_BUILDER
#include "checksums.h"
#include "core.h"
#include "core/os/io.h"
#include <stdio.h>

static char* get_checksum_ptr(const char* checksums_str, const char* cfile_rel_path)
{
    char* checksum_p = strstr(checksums_str, cfile_rel_path);
    if (checksum_p == NULL)
        return NULL;
    // advance pointer forward by the length of the path + ":", 
    // only getting the checksum
    checksum_p += (strlen(cfile_rel_path) + 1);
    return checksum_p;
}

bool get_checksum(const char* checksums_str, const char* cfile_rel_path, char* out_checksum)
{
    char* checksum_p = get_checksum_ptr(checksums_str, cfile_rel_path);
    snprintf(out_checksum, 33, checksum_p);
    out_checksum[32] = '\0';
    return true;
}

bool update_checksum(char* checksums_str, const char* cfile_rel_path, const char* new_checksum)
{
    char* checksum_p = NULL;
    while ((checksum_p = get_checksum_ptr(checksums_str, cfile_rel_path)) == NULL)
    {
        if (!add_checksum(checksums_str, cfile_rel_path, new_checksum))
        {
            LOG_ERR("Failed to add checksum.");
            return false;
        }
    }
    
    strncpy(checksum_p, new_checksum, 32);

    return true;
}

bool add_checksum(char* checksums_str, const char* cfile_rel_path, const char* checksum)
{
    char line_buf[MAX_PATH_LEN + 33];
    sprintf(line_buf, "%s:%s\n", cfile_rel_path, checksum);
    strcat(checksums_str, line_buf);
    // yes
    LOG_DEBUG("new checksum string: %s", checksums_str);
    return true;
}

#endif