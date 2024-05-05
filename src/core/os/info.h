#pragma once
#include "def.h"

typedef struct DisplayInfo {
    u16 screen_width;
    u16 screen_height;
} DisplayInfo;

// This should be constantly updated. Maybe.. who knows?
bool GDF_InitInfo();
f64 GDF_GetAbsoluteTime();
void GDF_Sleep(u64 ms);
DisplayInfo GetDisplayInfo();