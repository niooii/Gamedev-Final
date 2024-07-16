#pragma once

#include "core.h"
#include "engine/math/math_types.h"

typedef struct PointLight {
    vec3 pos;
    f32 strength;
} PointLight;