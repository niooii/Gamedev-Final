#pragma once

#include "core.h"
#include "engine/math/math.h"
#include "aabb.h"

typedef struct Physics_T* PhysicsEngine;

typedef struct PhysicsComponent {
    vec3 vel;
    vec3 accel;

    AxisAlignedBoundingBox aabb;
    bool grounded;
} PhysicsComponent;

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

/*
 * Creates a new physics component, and returns it for immediate modification.
 * It is up to the user to determine how to link this component to entities.
 */
PhysicsComponent* physics_create_component(PhysicsEngine engine);

typedef struct World World; 
bool physics_update(PhysicsEngine engine, World* world, f64 delta_time);