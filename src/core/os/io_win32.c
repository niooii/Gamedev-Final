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
    EXECUTABLE_PATH = malloc(40000);
    GetModuleFileName(NULL, EXECUTABLE_PATH, 40000);

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
    // ??
    register int i = 0;
    while(str[i] != '\0') 
    {
        if (str[i] == '/')
        {
            // printf("replacing slash");
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

char* GDF_GetAbsolutePath(const char* rel_path)
{
    // TODO! 
    // ReplaceFrontSlashWithBack
    TCHAR* dir = malloc(sizeof(TCHAR) * 4000);

    StringCchCopy(dir, 4000, EXECUTABLE_PATH);
    StringCchCat(dir, 4000, rel_path);

    ReplaceFrontSlashWithBack(dir);

    return dir;
}

// TODO! return once dynamic growbale array yes
GDF_DirInfo* GDF_GetDirInfo(const char* rel_path) 
{
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    TCHAR* tmp_dir = GDF_GetAbsolutePath(rel_path);
    TCHAR dir[4000];
    StringCchCopy(dir, 4000, tmp_dir);
    free(tmp_dir);
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
    TCHAR search_path[4000];
    StringCchCopy(search_path, 4000, dir);
    StringCchCat(search_path, 4000, TEXT("*"));

    hFind = FindFirstFile(search_path, &FindData);
    LOG_INFO("%s", FindData.cFileName);

    while(FindNextFile(hFind, &FindData))
    {       
        // y no work
        // if (strcmp(FindData.cFileName, ".") || strcmp(FindData.cFileName, "..")) 
        //     continue;
        // LOG_INFO("%s", FindData.cFileName);
        const char* full_path = GDF_StrcatNoOverwrite(dir, FindData.cFileName);
        LOG_INFO("found @ %s", full_path);
        bool is_dir = FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        // LOG_INFO("IS DIR: %d", is_dir);
    }
    return NULL;
}

bool GDF_MakeFile(const char* rel_path) {
    const char* path = GDF_GetAbsolutePath(rel_path);
    HANDLE h = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
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
    free(path);
    CloseHandle(h);
    return success;
}

bool GDF_MakeDir(const char* rel_path) {
    char* path = GDF_GetAbsolutePath(rel_path);
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
    free(path);

    // back to only 0 and 1s not some random value from win32 api
    return success != 0;
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