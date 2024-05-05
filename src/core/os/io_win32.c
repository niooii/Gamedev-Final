#include "io.h"

#ifdef OS_WINDOWS
#include <Windows.h>

void GDF_WriteConsole(const char* msg, u8 color)
{
    HANDLE stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(stdout, levels[color]);
    OutputDebugStringA(msg);
    u64 len = strlen(msg);
    WriteConsoleA(stdout, msg, (DWORD)len, 0, 0);    
}

#endif