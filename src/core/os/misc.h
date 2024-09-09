#pragma once
#include "core.h"

typedef struct GDF_DisplayInfo {
    u16 screen_width;
    u16 screen_height;
} GDF_DisplayInfo;

// This should be constantly updated. Maybe.. who knows?
bool GDF_InitInfo();
f64 GDF_GetAbsoluteTime();
GDF_DisplayInfo GDF_GetDisplayInfo();