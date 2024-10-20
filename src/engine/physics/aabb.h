#pragma once

#include "core.h"
#include "engine/math/math_types.h"

typedef struct AxisAlignedBoundingBox {
    vec3 min;
    vec3 max;
} AxisAlignedBoundingBox;