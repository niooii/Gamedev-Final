#include "world.h"

void world_create(World* out_world, WorldCreateInfo* create_info)
{
    out_world->chunk_simulate_distance = create_info->chunk_simulate_distance;
    i32 chunk_sim_distance = out_world->chunk_simulate_distance;
    out_world->ticks_per_sec = create_info->ticks_per_sec;

    out_world->since_last_tick = GDF_Stopwatch_Create();

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

void world_tick(World* world)
{
    LOG_INFO("something update world");
}