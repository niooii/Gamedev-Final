#include "io.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

static HWND console_window = NULL;
static const char* EXECUTABLE_PATH;

void GDF_IOInit() 
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

char** GDF_ListDirs(const char* rel_path) 
{
    return NULL;
}

char** GDF_GetDirStructure(const char* rel_path) 
{
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    TCHAR dir[4000];

    StringCchCopy(dir, 4000, EXECUTABLE_PATH);
    StringCchCat(dir, 4000, rel_path);
    // Find the last occurrence of '\'
    bool last_char_is_backslash = rel_path[strlen(rel_path)] == '\\' || rel_path[strlen(rel_path)] == '/';
    if (!last_char_is_backslash) {
        printf("last char on string %s is not / or \\\n", rel_path);
        size_t strlength = strlen(dir);
        *(dir + strlength) = '\\'; // add one ourself '\'
        *(dir + strlength + 1) = '\0'; // null terminate
    }
    LOG_DEBUG("something something %s", dir);
    TCHAR search_path[4000];
    StringCchCopy(search_path, 4000, dir);
    StringCchCat(search_path, 4000, TEXT("*"));
    LOG_DEBUG("%s", EXECUTABLE_PATH);

    hFind = FindFirstFile(search_path, &FindData);
    LOG_INFO("%s", FindData.cFileName);

    while(FindNextFile(hFind, &FindData))
    {
        // if (GetFileAttributesA(FindData))
        LOG_INFO("%s", FindData.cFileName);
        const char* full_path = GDF_StrcatNoOverwrite(dir, FindData.cFileName);
        LOG_INFO("found @ %s", full_path);
    }
    return NULL;
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


#endif