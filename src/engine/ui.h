#pragma once

#include "core.h"
#include "engine/math/math_types.h"

typedef struct Color {
    u8 r;
    u8 g;
    u8 b;
} Color;

typedef struct Text {
    const char* str;
    Color col;
    vec2 screen_pos;
} Text;
