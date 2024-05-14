#pragma once
#include "core.h"

#define GDF_WIN_CENTERED 32767

typedef struct GDF_Window {
    u16 id;
    void* internals;
} GDF_Window;

bool GDF_InitWindowing();
void GDF_ShutdownWindowing();
GDF_Window* GDF_CreateWindow(i16 x, i16 y, i16 w, i16 h, const char* title);
bool GDF_SetWindowPos(i16 dest_x, i16 dest_y);
bool GDF_SetWindowSize(i16 w, i16 h);
bool GDF_PumpMessages();
bool GDF_DestroyWindow(GDF_Window* window);