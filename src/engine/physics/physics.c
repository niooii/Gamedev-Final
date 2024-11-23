#include "physics.h"
#include "game/world/world.h"
#include "core/collections/list.h"

typedef struct Physics_T {
    PhysicsComponent** components;
    
    f32 terminal_velocity;
    f32 air_drag;
    f32 ground_drag;
    vec3 gravity;
    bool gravity_active; 
} Physics_T;

PhysicsEngine physics_init(PhysicsCreateInfo create_info)
{
    PhysicsEngine physics = GDF_Malloc(sizeof(Physics_T), GDF_MEMTAG_APPLICATION);
    physics->components = GDF_LIST_Reserve(PhysicsComponent*, 32);
    physics->gravity = create_info.gravity;
    physics->gravity_active = create_info.gravity_active;
    physics->air_drag = create_info.air_drag;
    physics->ground_drag = create_info.ground_drag;
    physics->terminal_velocity = create_info.terminal_velocity;

    return physics;
}

PhysicsComponent* physics_create_component(PhysicsEngine engine)
{
    PhysicsComponent* component = GDF_Malloc(sizeof(PhysicsComponent), GDF_MEMTAG_UNKNOWN);
    GDF_LIST_Push(engine->components, component);

    return component;
}

bool physics_update(PhysicsEngine engine, World* world, f64 dt)
{   
    // TODO! optimize, look into SIMD
    vec3 effective_gravity = engine->gravity_active ? engine->gravity : vec3_zero();
    vec3 net_accel;
    
    u32 len = GDF_LIST_GetLength(engine->components);
    for (u32 i = 0; i < len; i++)
    {
        PhysicsComponent* comp = engine->components[i];

        f32 drag = comp->grounded ? engine->ground_drag : engine->air_drag;

        // TODO! this aint quite right..
        if (engine->gravity_active)
        {
            comp->vel.x *= (drag * (1 - dt));
            comp->vel.z *= (drag * (1 - dt));
        }
        
        // gravity will be handling the drag
        // if (!comp->grounded)
        // {
        //     comp->vel.y *= engine->air_drag;
        // }

        net_accel = vec3_add(comp->accel, effective_gravity);
        
        vec3 deltas = vec3_new(
            comp->vel.x * dt + 0.5f * net_accel.x * dt * dt,
            comp->vel.y * dt + 0.5f * net_accel.y * dt * dt,
            comp->vel.z * dt + 0.5f * net_accel.z * dt * dt
        );

        comp->vel.x = comp->vel.x + net_accel.x * dt;
        // cap velocity gain at a terminal velocity
        // TODO! should this be for all axis? (and both y directions?)
        if (comp->vel.y > engine->terminal_velocity)
        {
            f32 t_vy = comp->vel.y + net_accel.y * dt;
            if (t_vy < engine->terminal_velocity)
                comp->vel.y = engine->terminal_velocity;
            else
                comp->vel.y = t_vy;
        }
        comp->vel.z = comp->vel.z + net_accel.z * dt;

        // TODO! eliminate phasing through blocks at high velocities
        // with raycasting

        AxisAlignedBoundingBox translated_aabb = comp->aabb;
        aabb_translate(&translated_aabb, deltas);
        
        BlockTouchingResult results[64];
        u32 results_len = world_get_blocks_touching(
            world, 
            &translated_aabb,
            results,
            sizeof(results) / sizeof(*results)
        );

        comp->grounded = false;

        for (u32 i = 0; i < results_len; i++)
        {
            BlockTouchingResult* r = results + i;
            // LOG_DEBUG(
            //     "FOUND TOUCHING BLOCK at: %f, %f, %f", 
            //     r->box.min.x,
            //     r->box.min.y,
            //     r->box.min.z
            // );
            
            if (aabb_intersects(&translated_aabb, &r->box))
            {
                vec3 resolution = aabb_get_intersection_resolution(&translated_aabb, &r->box);
                vec3_add_to(&deltas, resolution);
                aabb_translate(&translated_aabb, resolution);
                // zero velocity and shi
                if (resolution.x != 0)
                {
                    comp->vel.x = 0;
                }
                else if (resolution.y != 0)
                {
                    comp->vel.y = 0;
                    if (resolution.y > 0)
                        comp->grounded = true;
                }
                else 
                {
                    comp->vel.z = 0;
                }
            }
        }

        aabb_translate(&comp->aabb, deltas);

        if (comp->grounded)
        {
            // more drag
        }
        else
        {
            // less drag
        }
    }

    return true;
}

