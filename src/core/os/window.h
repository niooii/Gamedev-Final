#pragma once
#include "core.h"
#include "core/os/io.h"
#include "core/input.h"
#include "render/vulkan/vk_os.h"
#include "core/containers/list.h"

#define GDF_WIN_CENTERED 32767

// TODO! support muiltiple windows later (or completely redesign to only support 1).
// this is a scaffold for multiple windows but
// does not actually support it, the static MAIN_WINDOW in every
// implementation is the only window as of now.
// one possible way is to pass in a GDF_Window* to the GDF_InitRenderer.
// actually thats a fire idea wtf why didnt i do that ill do it later
typedef struct GDF_Window {
    u16 id;
    void* internals;
} GDF_Window;

bool GDF_InitWindowing();
void GDF_ShutdownWindowing();
GDF_Window* GDF_CreateWindow(i16 x, i16 y, i16 w, i16 h, const char* title);
bool GDF_SetWindowPos(i16 dest_x, i16 dest_y);
// should be client width and height
bool GDF_SetWindowSize(i16 w, i16 h);
void GDF_GetWindowSize(i16* w, i16* h);
bool GDF_PumpMessages();
bool GDF_DestroyWindow(GDF_Window* window);
GDF_Window* GDF_GetMainWindow();