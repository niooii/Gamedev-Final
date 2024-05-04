#pragma once
#include "def.h"

typedef enum Color {
    GDF_STYLE_WHITE
} Color;

void GDF_WriteConsole(const char* msg, Color color);