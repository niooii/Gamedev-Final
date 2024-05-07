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
        // Find the second last occurrence of '\'
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

char** GDF_ListFiles(const char* rel_path) 
{
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    TCHAR dir[4000];

    StringCchCopy(dir, 4000, EXECUTABLE_PATH);
    StringCchCat(dir, 4000, rel_path);
    StringCchCat(dir, 4000, TEXT("\\*.*"));
    LOG_DEBUG("%s", EXECUTABLE_PATH);

    hFind = FindFirstFile(dir, &FindData);
    LOG_INFO("%s", FindData.cFileName);

    while(FindNextFile(hFind, &FindData))
    {
        LOG_INFO("%s", FindData.cFileName);
    }
    return NULL;
}

#endif