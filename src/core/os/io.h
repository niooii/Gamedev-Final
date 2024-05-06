#pragma once
#include "def.h"

typedef enum GDF_TextColor {
    GDF_STYLE_WHITE
} GDF_TextColor;

void GDF_ShowConsole();
void GDF_HideConsole();
void GDF_WriteConsole(const char* msg, u8 color);

