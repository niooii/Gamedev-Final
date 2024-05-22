#pragma once
#include "core.h"
#include "app_settings.h"
#define MB_BYTES 1048576
#define KB_BYTES 1024

#define MAX_PATH_LEN 4000

typedef struct GDF_DirInfoNode {
    const char* name;
    const char* full_path;
    bool is_directory;
} GDF_DirInfoNode;

typedef struct GDF_DirInfo {
    GDF_DirInfoNode* nodes;
    size_t num_nodes;
} GDF_DirInfo;

void GDF_InitIO();
void GDF_ShowConsole();
void GDF_HideConsole();
void GDF_WriteConsole(const char* msg, u8 color);
// must be freed
void GDF_GetAbsolutePath(const char* rel_path, char* out_buf);
// MUST BE DESTROYED with GDF_FreeDirInfo
// Example:
// GDF_GetDirInfo("worlds") // gets ./worlds folder form executable folder
// Also valid:
// GDF_GetDirInfo("worlds/")
// GDF_GetDirInfo("worlds\\")
// GDF_GetDirInfo("worlds/players")
// ASSUMES THE RELATIVE PATH IS A VALID DIRECTORY
// RETURNS NULL
GDF_DirInfo* GDF_GetDirInfo(const char* rel_path);
bool GDF_MakeFile(const char* rel_path);
bool GDF_MakeDir(const char* rel_path);
// WILL OVERWRITE CONTENTS OF FILE
bool GDF_WriteFile(const char* rel_path, const char* data);
bool GDF_ReadFile(const char* rel_path, char* out_buf, size_t bytes_to_read);
char* GDF_StrcatNoOverwrite(const char* s1, const char* s2);

// free resources
void GDF_FreeDirInfo(GDF_DirInfo* dir_info);