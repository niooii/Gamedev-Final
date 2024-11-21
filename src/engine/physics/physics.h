#pragma once

#include "core.h"
#include "engine/math/math.h"
#include "aabb.h"

typedef struct Physics_T* PhysicsEngine;

typedef struct PhysicsComponent {
    vec3 vel;
    vec3 accel;

    AxisAlignedBoundingBox aabb;    
} PhysicsComponent;

typedef struct PhysicsCreateInfo {
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