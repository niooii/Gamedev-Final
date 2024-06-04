#pragma once

#include "def.h"

typedef union vec2_u {
    f32 elements[2];
    struct {
        f32 x;
        f32 y;
    };
} vec2;

typedef union vec3_u {
    f32 elements[3];
    struct {
        f32 x;
        f32 y;
        f32 z;
    };
} vec3;

typedef union vec4_u {
    f32 elements[4];
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
} vec4;

typedef vec4 quaternion;