#pragma once
#include "core.h"

typedef struct GDF_DirInfoNode {
    const char* name;
    const char* full_path;
    bool is_directory;
} GDF_DirInfoNode;

typedef struct GDF_DirInfo {
    GDF_DirInfoNode* nodes;
    size_t num_nodes;
} GDF_DirInfo;

void GDF_IOInit();
void GDF_ShowConsole();
void GDF_HideConsole();
void GDF_WriteConsole(const char* msg, u8 color);
// must be freed
char* GDF_GetAbsolutePath(const char* rel_path);
char* GDF_ReadFromFile(const char* rel_path);
// Example:
// GDF_GetDirInfo("worlds") // gets ./worlds folder form executable folder
// Also valid:
// GDF_GetDirInfo("worlds/")
// GDF_GetDirInfo("worlds\\")
// GDF_GetDirInfo("worlds/players")
// ASSUMES THE RELATIVE PATH IS A VALID DIRECTORY
GDF_DirInfo* GDF_GetDirInfo(const char* rel_path);
char* GDF_StrcatNoOverwrite(const char* s1, const char* s2);

// free resources
void GDF_FreeDirInfo(GDF_DirInfo* dir_info);