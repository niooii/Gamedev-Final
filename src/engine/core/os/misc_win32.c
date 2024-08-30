#include "misc.h"

#ifdef OS_WINDOWS
#include <Windows.h>

static f64 clock_freq;
static LARGE_INTEGER start_time;

bool GDF_InitInfo()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    clock_freq = 1.0 / (f64)freq.QuadPart;
    QueryPerformanceCounter(&start_time);
    return true;
}

f64 GDF_GetAbsoluteTime()
{
    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (f64)now_time.QuadPart * clock_freq;
}

GDF_DisplayInfo GDF_GetDisplayInfo()
{
    GDF_DisplayInfo di = {
        .screen_width = GetSystemMetrics(SM_CXSCREEN),
        .screen_height = GetSystemMetrics(SM_CYSCREEN)
    };

    return di;
}

#endif