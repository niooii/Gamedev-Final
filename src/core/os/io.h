#pragma once
#include "def.h"

typedef enum GDF_TextColor {
    GDF_STYLE_WHITE
} GDF_TextColor;
// https://dev.to/mikkel250/reading-files-contents-in-c-ma9#:~:text=Reading%20characters%20from%20files%20with,read%20newline%20characters%20as%20well.
void GDF_ShowConsole();
void GDF_HideConsole();
void GDF_WriteConsole(const char* msg, u8 color);
char* GDF_ReadFromFile(const char* rel_path);