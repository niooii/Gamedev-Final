#pragma once

#include "def.h"
#include "math_types.h"

#define PI 3.14159265358979323846f
#define PI_2 2.0f * K_PI
#define HALF_PI 0.5f * K_PI
#define QUARTER_PI 0.25f * K_PI
#define ONE_OVER_PI 1.0f / K_PI
#define ONE_OVER_TWO_PI 1.0f / K_PI_2
#define SQRT_TWO 1.41421356237309504880f
#define SQRT_THREE 1.73205080756887729352f
#define SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define DEG_TO_RAD K_PI / 180.0f
#define RAD_TO_DEG 180.0f / K_PI

// The multiplier to convert seconds to milliseconds.
#define SEC_TO_MS 1000.0f

// The multiplier to convert milliseconds to seconds.
#define MS_TO_SEC 0.001f

// A huge number that should be larger than any valid number used.
#define INFINITY 1e30f

// Smallest positive number where 1.0 + FLOAT_EPSILON != 0
#define FLOAT_EPSILON 1.192092896e-07f