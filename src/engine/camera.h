#pragma once

#include "core.h"
#include "engine/math/math_types.h"

typedef struct Camera {
    vec3 pos;
    f32 pitch;
    f32 yaw;
} Camera;