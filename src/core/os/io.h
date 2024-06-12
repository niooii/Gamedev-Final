#pragma once
#include "core.h"
#include "client/client_settings.h"
#define MB_BYTES 1048576
#define KB_BYTES 1024

#define MAX_PATH_LEN 512

typedef struct GDF_DirInfoNode {
    char* name;
    char* full_path;
    bool is_directory;
} GDF_DirInfoNode;

typedef struct GDF_DirInfo {
    GDF_DirInfoNode** nodes;
    size_t num_nodes;
} GDF_DirInfo;

void GDF_InitIO();
void GDF_ShowConsole();
void GDF_HideConsole();
void GDF_WriteConsole(const char* msg, u8 color);
void GDF_GetAbsolutePath(const char* rel_path, char* out_buf);
// gets path of resource in a resources/ directory
// ex: resources/icon.ico can be accessed by GDF_GetResourcePath("icon.ico", buf)
void GDF_GetResourcePath(const char* rel_path, char* out_buf);
// if the path is outside the directory of the executable
// out_buf will be set to NULL. 
void GDF_GetRelativePath(const char* abs_path, char* out_buf);
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
bool GDF_MakeDirAbs(const char* abs_path);
// WILL OVERWRITE CONTENTS OF FILE
bool GDF_WriteFile(const char* rel_path, const char* data);
bool GDF_ReadFile(const char* rel_path, char* out_buf, size_t bytes_to_read);
// must be freed with GDF_Free
// returns NULL on error
char* GDF_ReadFileExactLen(const char* rel_path);
// returns NULL on error
u8* GDF_ReadBytesExactLen(const char* rel_path);
// returns false if overwrite_existing is false and a file already exists at dest_path,
// or the src_path or dest_path is invalid.
bool GDF_CopyFile(const char* src_path, const char* dest_path, bool overwrite_existing);
// the size of the file in bytes
u64 GDF_GetFileSize(const char* rel_path);
u64 GDF_GetFileSizeAbs(const char* abs_path);
char* GDF_StrcatNoOverwrite(const char* s1, const char* s2);
// must be freed with GDF_Free
char* GDF_StrDup(const char* str);

// free resources
void GDF_FreeDirInfo(GDF_DirInfo* dir_info);