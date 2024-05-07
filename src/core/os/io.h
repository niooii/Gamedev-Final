#pragma once
#include "core.h"

typedef struct GDF_FileInfo {
    const char* name;
    const char* full_path;
    bool is_directory;
} GDF_FileInfo;

void GDF_IOInit();
void GDF_ShowConsole();
void GDF_HideConsole();
void GDF_WriteConsole(const char* msg, u8 color);
char* GDF_ReadFromFile(const char* rel_path);
// Example:
// GDF_GetDirStructure("worlds") // gets ./worlds folder form executable folder
char** GDF_GetDirStructure(const char* rel_path);
char* GDF_StrcatNoOverwrite(const char* s1, const char* s2);