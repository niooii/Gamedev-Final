#pragma once

#include "core.h"
#include "engine/math/math.h"

typedef struct AxisAlignedBoundingBox {
    vec3 min;
    vec3 max;
} AxisAlignedBoundingBox;

// Returns whether an aabb is colliding with another aabb->
FORCEINLINE bool aabb_collides(AxisAlignedBoundingBox* a, AxisAlignedBoundingBox* b) 
{
    return a->min.x <= b->max.x &&
    a->max.x >= b->min.x &&
    a->min.y <= b->max.y &&
    a->max.y >= b->min.y &&
    a->min.z <= b->max.z &&
    a->max.z >= b->min.z;
}

// Translates an aabb by the given translation vector.
FORCEINLINE void aabb_translate(AxisAlignedBoundingBox* a, vec3 t)
{
    vec3_add_to(&a->min, t);
    vec3_add_to(&a->max, t);
}