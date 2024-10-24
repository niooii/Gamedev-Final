#include "physics.h"

#include "core/collections/list.h"

typedef struct Physics_T {
    PhysicsComponent* components;
    
    vec3 gravity;
    bool gravity_active; 
} Physics_T;

PhysicsEngine physics_init(PhysicsCreateInfo create_info)
{
    PhysicsEngine physics = GDF_Malloc(sizeof(Physics_T), GDF_MEMTAG_APPLICATION);
    physics->components = GDF_LIST_Reserve(PhysicsComponent, 32);
    physics->gravity = create_info.gravity;
    physics->gravity_active = create_info.gravity_active;

    return physics;
}

PhysicsComponent* physics_create_component(PhysicsEngine engine)
{
    PhysicsComponent component = {
        .aabb = 0,
        .accel = 0,
        .pos = 0,
        .vel = 0
    };
    GDF_LIST_Push(engine->components, component);

    return &engine->components[GDF_LIST_GetLength(engine->components) - 1];
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
        PhysicsComponent* comp = engine->components + i;

        net_accel = vec3_add(comp->accel, effective_gravity);
        
        comp->pos.x += comp->vel.x * dt + 0.5f * net_accel.x * dt * dt;
        comp->pos.y += comp->vel.y * dt + 0.5f * net_accel.y * dt * dt;
        comp->pos.z += comp->vel.z * dt + 0.5f * net_accel.z * dt * dt;
        
        comp->vel.x = comp->vel.x + net_accel.x * dt;
        comp->vel.y = comp->vel.y + net_accel.y * dt;
        comp->vel.z = comp->vel.z + net_accel.z * dt;
    }

    return true;
}

