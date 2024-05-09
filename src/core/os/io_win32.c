#include "io.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

static HWND console_window = NULL;
static const char* EXECUTABLE_PATH;

void GDF_InitIO() 
{
    EXECUTABLE_PATH = malloc(MAX_PATH_LEN);
    GetModuleFileName(NULL, EXECUTABLE_PATH, MAX_PATH_LEN);

    // Find the last occurrence of '\'
    char* lastBackslash = strrchr(EXECUTABLE_PATH, '\\');
    if (lastBackslash != NULL) {
        *(lastBackslash+1) = '\0'; // Null-terminate the string at the last '\'
    }
}

void GDF_ShowConsole()
{
    if (console_window == NULL)
    {
        console_window = GetConsoleWindow();
    }

    ShowWindow(console_window, SW_SHOW);
}

static void ReplaceFrontSlashWithBack(char* str) 
{
    int i = 0;
    while(str[i] != '\0') 
    {
        if (str[i] == '/')
        {
            str[i] = '\\';
        }
        i++;
    }
}

void GDF_HideConsole()
{
    if (console_window == NULL)
    {
        console_window = GetConsoleWindow();
    }

    ShowWindow(console_window, SW_HIDE);
}

void GDF_WriteConsole(const char* msg, u8 color)
{
    HANDLE stdout_ = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(stdout_, levels[color]);

    OutputDebugStringA(msg);
    
    u64 len = strlen(msg);
    WriteConsoleA(stdout_, msg, (DWORD)len, 0, 0);    
}

void GDF_GetAbsolutePath(const char* rel_path, char* out_buf)
{
    // TODO! 
    // magic number :devio:
    StringCchCopy(out_buf, MAX_PATH_LEN, EXECUTABLE_PATH);
    StringCchCat(out_buf, MAX_PATH_LEN, rel_path);

    ReplaceFrontSlashWithBack(out_buf);
}

// TODO! return once dynamic growbale array yes
GDF_DirInfo* GDF_GetDirInfo(const char* rel_path) 
{
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    char* tmp_dir[MAX_PATH_LEN];
    GDF_GetAbsolutePath(rel_path, tmp_dir);
    TCHAR dir[MAX_PATH_LEN];
    StringCchCopy(dir, MAX_PATH_LEN, tmp_dir);
    // Find the last occurrence of '\'
    bool last_char_is_backslash = rel_path[strlen(rel_path) - 1] == '\\';
    bool last_char_is_slash = rel_path[strlen(rel_path) - 1] == '/'; 
    if (!last_char_is_backslash && !last_char_is_slash) 
    {
        size_t strlength = strlen(dir);
        *(dir + strlength) = '\\'; // add one ourself '\'
        *(dir + strlength + 1) = '\0'; // null terminate
    }
    else if (last_char_is_slash)
    {
        size_t strlength = strlen(dir);
        *(dir + strlength - 1) = (char)'\\'; // replace with '\'
    }
    TCHAR search_path[MAX_PATH_LEN];
    StringCchCopy(search_path, MAX_PATH_LEN, dir);
    StringCchCat(search_path, MAX_PATH_LEN, TEXT("*"));

    hFind = FindFirstFile(search_path, &FindData);
    // for some reason this never triggers. it always works.
    // even when given a random ass path it somehow manages
    // to return a valid handle and quite frankly, i am
    // very pressed about it. according to the docs, this 
    // is SUPPOSED to work. but of course, why would anything
    // work as documented, right? i am quite distressed about
    // the lack of error handling in the FindFirstFileW function
    // and will pray that this functionality will remain the same
    // in future versions of windows. 
    // if (FindData.cFileName == INVALID_HANDLE_VALUE)
    // {
    //     LOG_WARN("Could not search directory %s", search_path);
    //     return NULL;
    // }
    int found_files = 0;
    while(FindNextFile(hFind, &FindData))
    {       
        // y no work
        // if (strcmp(FindData.cFileName, ".") || strcmp(FindData.cFileName, "..")) 
        //     continue;
        LOG_INFO("%s", FindData.cFileName);
        const char* full_path = GDF_StrcatNoOverwrite(dir, FindData.cFileName);
        // LOG_INFO("found @ %s", full_path);
        bool is_dir = FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        // LOG_INFO("IS DIR: %d", is_dir);
        found_files++;
    }
    FindClose(hFind);

    if (found_files == 0)
    {
        // we should have found the .. directory and
        // found_files wouldve been 1, so this
        // can only mean the directory is invalid.
        LOG_WARN("Could not search directory: %s", dir);
        return NULL;
    }

    return NULL;
}

bool GDF_MakeFile(const char* rel_path) {
    const char* path[MAX_PATH_LEN];
    GDF_GetAbsolutePath(rel_path, path);
    HANDLE h = CreateFile(path, 0, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
    bool success = h != INVALID_HANDLE_VALUE;
    if (!success)
    {
        if (GetLastError() == ERROR_FILE_EXISTS)
        {
            LOG_WARN("File already exists: %s", path);
        }
        else
        {
            LOG_WARN("Failed to create file: %s", path);
        }
    }
    else
    {
        LOG_INFO("Created file: %s", path);
    }
    CloseHandle(h);
    return success;
}

bool GDF_MakeDir(const char* rel_path) {
    char* path[MAX_PATH_LEN];
    GDF_GetAbsolutePath(rel_path, path);
    bool success = CreateDirectoryA(path, NULL);
    // TODO! replace with custom allocator
    if (!success) 
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS) 
        {
            LOG_WARN("Directory already exists: %s", path);
        }
        else
        {
            LOG_WARN("Failed to create directory: %s", path);
        }
    }
    else
    {
        LOG_INFO("Created directory: %s", path);
    }

    // back to only 0 and 1s not some random value from win32 api
    return success != 0;
}

bool GDF_WriteFile(const char* rel_path, const char* data) {
    const char* path[MAX_PATH_LEN];
    GDF_GetAbsolutePath(rel_path, path);
    HANDLE h = CreateFile(path, GENERIC_WRITE, 0, 0, TRUNCATE_EXISTING, 0, 0);
    bool success = h != INVALID_HANDLE_VALUE;
    if (!success)
    {   
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            LOG_ERR("Could not write to non-existent file: %s", path);
        }
        else
        {
            LOG_WARN("Unknown error while opening write handle to file: %s", path);
        }
        return false;
    }
    bool w_success = WriteFile(h, data, strlen(data), NULL, NULL);
    if (w_success)
    {
        LOG_INFO("Wrote to file: %s", path);
    }
    else
    {
        LOG_ERR("Unknown error while writing to file: %s", path);
    }
    CloseHandle(h);
    return w_success;
}

// MUST CALL FREE AFTER USE
char* GDF_StrcatNoOverwrite(const char* s1,const char* s2)
{
  char* p = malloc(strlen(s1) + strlen(s2) + 1);

  char* start = p;
  if (p != NULL)
  {
       while (*s1 != '\0')
       *p++ = *s1++;
       while (*s2 != '\0')
       *p++ = *s2++;
      *p = '\0';
 }

  return start;
}

void GDF_FreeDirInfo(GDF_DirInfo* dir_info)
{
    for (int i = 0; i < dir_info->num_nodes; i++)
    {
        GDF_DirInfoNode node = dir_info->nodes[i];
        free(node.full_path);
        free(node.name);
    }

    // TODO! replace with allocator
    free(dir_info);
}

#endif