#include "time.h"

#ifdef OS_WINDOWS
#include <Windows.h>

static f64 clock_freq;
static LARGE_INTEGER start_time;

bool GDF_InitTimeSystem()
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

void GDF_Sleep(u64 ms)
{
    Sleep(ms);
}

#endif