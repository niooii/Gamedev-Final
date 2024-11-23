#pragma once

#include "core.h"
#include "engine/physics/aabb.h"

typedef enum ENTITY_TYPE {
    ENTITY_TYPE_BASE = 0,
    // "parent" is safe to be casted to HumanoidEntity*
    ENTITY_TYPE_HUMANOID
} ENTITY_TYPE;

typedef struct Entity {
    f32 health;
    bool damagable;
    ENTITY_TYPE type;
    void* parent;

    vec3 vel;
    vec3 accel;

    AxisAlignedBoundingBox aabb;
    bool grounded;
} Entity;