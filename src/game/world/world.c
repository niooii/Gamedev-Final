#include "world.h"

uint32_t chunk_hash(const u8* data, u32 len) {
    ChunkCoord* coord = (ChunkCoord*)(data);

    // primes n stuff
    const u32 p1 = 73856093u;  
    const u32 p2 = 19349663u;
    const u32 p3 = 83492791u;

    u32 h1 = (u32)(coord->x) * p1;
    u32 h2 = (u32)(coord->y) * p2;
    u32 h3 = (u32)(coord->z) * p3;
    
    return h1 ^ h2 ^ h3;
}

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
    out_world->chunks = GDF_HashmapWithHasher(ChunkCoord, Chunk, chunk_hash, false);

    out_world->world_update_stopwatch = GDF_StopwatchCreate();

    // Create da chunks
    for (i32 chunk_x = -5; chunk_x <= 5; chunk_x++)
    {
        for (i32 chunk_y = -5; chunk_y <= 5; chunk_y++)
        {
            for (i32 chunk_z = -5; chunk_z <= 5; chunk_z++)
            {
                ChunkCoord c = {
                    .x = chunk_x,
                    .y = chunk_y,
                    .z = chunk_z
                };
                Chunk ch = {};
                GDF_HashmapInsert(out_world->chunks, &c, &ch);
            }
        }
    }

    LOG_ERR("amount entries: %d", GDF_HashmapLen(out_world->chunks));
}

void world_update(World* world, f64 dt)
{
    physics_update(world->physics, dt);
}

Chunk* world_get_chunk(World* world, ChunkCoord coord)
{
    Chunk* c = GDF_HashmapGet(world->chunks, &coord);
    if (c == NULL)
    {
        Chunk t = {};
        chunk_init(&t);
        c = GDF_HashmapInsert(world->chunks, &coord, &t);
        generator_gen_chunk(&world->generator, world, coord, c);
    }

    return c;
}

void world_tick(World* world)
{
    LOG_INFO("something tick world");
}