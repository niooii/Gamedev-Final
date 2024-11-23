#pragma once

#include "core.h"
#include "engine/math/math.h"
#include "aabb.h"
#include "game/entity/entity.h"

typedef struct Physics_T* PhysicsEngine;

typedef struct PhysicsCreateInfo {
    f32 ground_drag;
    f32 air_drag;
    // TODO! this is only for negative Y, so
    // only negative numbers will work
    f32 terminal_velocity;
    vec3 gravity;
    bool gravity_active;
} PhysicsCreateInfo;

PhysicsEngine physics_init(PhysicsCreateInfo create_info);

void physics_add_entity(PhysicsEngine engine, Entity* entity);

typedef struct World World; 
bool physics_update(PhysicsEngine engine, World* world, f64 delta_time);