#include "world.h"

void world_create(World* out_world, WorldCreateInfo* create_info)
{
    PhysicsCreateInfo physics_info = {
        .gravity = {0, -9.81, 0},
        .gravity_active = true
    };
    out_world->physics = physics_init(physics_info);
    out_world->chunk_simulate_distance = create_info->chunk_simulate_distance;
    i32 chunk_sim_distance = out_world->chunk_simulate_distance;
    out_world->ticks_per_sec = create_info->ticks_per_sec;

    out_world->world_update_stopwatch = GDF_StopwatchCreate();

    // Create da chunks
    for (i32 chunk_x = -chunk_sim_distance; chunk_x <= chunk_sim_distance; chunk_x++)
    {
        for (i32 chunk_y = -chunk_sim_distance; chunk_y <= chunk_sim_distance; chunk_y++)
        {
            for (i32 chunk_z = -chunk_sim_distance; chunk_z <= chunk_sim_distance; chunk_z++)
            {
                
            }
        }
    }
}

void world_update(World* world, f64 dt)
{
    physics_update(world->physics, dt);
}

void world_tick(World* world)
{
    LOG_INFO("something tick world");
}