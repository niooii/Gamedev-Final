#include "physics.h"

#include "core/collections/list.h"

typedef struct Physics_T {
    PhysicsComponent** components;
    
    vec3 gravity;
    bool gravity_active; 
} Physics_T;

PhysicsEngine physics_init(PhysicsCreateInfo create_info)
{
    PhysicsEngine physics = GDF_Malloc(sizeof(Physics_T), GDF_MEMTAG_APPLICATION);
    physics->components = GDF_LIST_Reserve(PhysicsComponent*, 32);
    physics->gravity = create_info.gravity;
    physics->gravity_active = create_info.gravity_active;

    return physics;
}

PhysicsComponent* physics_create_component(PhysicsEngine engine)
{
    PhysicsComponent* component = GDF_Malloc(sizeof(PhysicsComponent), GDF_MEMTAG_UNKNOWN);
    GDF_LIST_Push(engine->components, component);

    return component;
}

// TODO! collision
bool physics_update(PhysicsEngine engine, f64 dt)
{   
    // TODO! optimize, look into SIMD
    vec3 effective_gravity = engine->gravity_active ? engine->gravity : vec3_zero();
    vec3 net_accel;
    
    u32 len = GDF_LIST_GetLength(engine->components);
    for (u32 i = 0; i < len; i++)
    {
        PhysicsComponent* comp = engine->components[i];

        net_accel = vec3_add(comp->accel, effective_gravity);
        
        vec3 deltas = vec3_new(
            comp->vel.x * dt + 0.5f * net_accel.x * dt * dt,
            comp->vel.y * dt + 0.5f * net_accel.y * dt * dt,
            comp->vel.z * dt + 0.5f * net_accel.z * dt * dt
        );

        // Stop entities at Y = 0 for now..
        if (comp->aabb.min.y <= 0)
        {
            comp->vel.y = MAX(0, comp->vel.y);
            deltas.y = MAX(0, deltas.y);
            comp->aabb.min.y = 0;
        }
        
        aabb_translate(&comp->aabb, deltas);
        
        comp->vel.x = comp->vel.x + net_accel.x * dt;
        comp->vel.y = comp->vel.y + net_accel.y * dt;
        comp->vel.z = comp->vel.z + net_accel.z * dt;
    }

    return true;
}

