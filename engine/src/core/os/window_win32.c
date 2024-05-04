#include "window.h"

#ifdef OS_WINDOWS

bool GDF_CreateWindow(i16 x, i16 y, i16 w, i16 h, const char* title) 
{
    return false;
}

bool GDF_SetWindowPos(i16 dest_x, i16 dest_y)
{
    return false;
}

bool GDF_SetWindowSize(i16 w, i16 h)
{
    return false;
}

bool GDF_DestroyWindow()
{
    return false;
}

#endif