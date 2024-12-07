#include "world.h"
#include "game/events.h"

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

static bool __on_block_touch(u16 event_code, void* sender, void* listener_instance, GDF_EventContext ctx)
{
    Block* block = (Block*)sender;
    Entity* entity = (Entity*)(ctx.data.u64[0]);
    if (entity->type != ENTITY_TYPE_HUMANOID)
        return false;

    HumanoidEntity* hum = entity->parent;

    return false;
}

void world_create(World* out_world, WorldCreateInfo* create_info)
{
    PhysicsCreateInfo physics_info = {
        .gravity = {0, -20, 0},
        .gravity_active = true,
        .air_drag = 3.f,
        .ground_drag = 12.f,
        .terminal_velocity = -50.f
    };
    out_world->physics = physics_init(physics_info);
    out_world->chunk_simulate_distance = create_info->chunk_simulate_distance;
    i32 chunk_sim_distance = out_world->chunk_simulate_distance;
    out_world->ticks_per_sec = create_info->ticks_per_sec;
    out_world->chunks = GDF_HashmapWithHasher(ChunkCoord, Chunk, chunk_hash, false);
    out_world->humanoids = GDF_LIST_Reserve(HumanoidEntity*, 32);

    out_world->world_update_stopwatch = GDF_StopwatchCreate();

    // Create chunks
    for (i32 chunk_x = -2; chunk_x <= 2; chunk_x++)
    {
        for (i32 chunk_y = 0; chunk_y < 1; chunk_y++)
        {
            for (i32 chunk_z = -2; chunk_z <= 2; chunk_z++)
            {
                ChunkCoord c = {
                    .x = chunk_x,
                    .y = chunk_y,
                    .z = chunk_z
                };
                world_get_or_create_chunk(out_world, c);
            }
        }
    }

    GDF_ASSERT(GDF_EVENT_Register(GDF_EVENT_BLOCK_TOUCHED, NULL, __on_block_touch));
}

HumanoidEntity* world_create_humanoid(World* world)
{
    HumanoidEntity* hum = GDF_Malloc(sizeof(HumanoidEntity), GDF_MEMTAG_GAME);

    GDF_LIST_Push(world->humanoids, hum);

    hum->base.type = ENTITY_TYPE_HUMANOID;
    hum->base.parent = hum;

    return hum;
}

void world_update(World* world, f64 dt)
{
    u32 num_humanoids = GDF_LIST_GetLength(world->humanoids);
    for (u32 i = 0; i < num_humanoids; i++)
        humanoid_entity_update(world->humanoids[i]);
    physics_update(world->physics, world, dt);
}

Chunk* world_get_or_create_chunk(World* world, ChunkCoord coord)
{
    Chunk* c = GDF_HashmapGet(world->chunks, &coord);
    if (c == NULL)
    {
        Chunk t = {};
        chunk_init(&t);
        c = GDF_HashmapInsert(world->chunks, &coord, &t);
        if (c == NULL)
        {
            LOG_WARN("WOMP WOMP");
        } 
        
        if (!generator_gen_chunk(&world->generator, world, coord, c))
        {
            LOG_ERR("Failed to create chunk");
            return NULL;
        }
        GDF_EVENT_Fire(GDF_EVENT_CHUNK_LOAD, c, (GDF_EventContext){});
    }

    return c;
}

void world_destroy_block(World* world, vec3 block_world_pos, Block* destroyed_block)
{
    ChunkCoord cc = world_pos_to_chunk_coord(block_world_pos);
    u8 block_x = FLOOR(block_world_pos.x - cc.x * CHUNK_SIZE_XZ);
    u8 block_y = FLOOR(block_world_pos.y - cc.y * CHUNK_SIZE_XZ);
    u8 block_z = FLOOR(block_world_pos.z - cc.z * CHUNK_SIZE_XZ);
}

Block* world_set_block(World* world, BlockCreateInfo* create_info)
{

}

u32 world_get_blocks_touching(
    World* world, 
    AxisAlignedBoundingBox* aabb, 
    BlockTouchingResult* result_arr,
    u32 result_arr_size
)
{
    f32 min_x = FLOOR(aabb->min.x);
    f32 min_y = FLOOR(aabb->min.y);
    f32 min_z = FLOOR(aabb->min.z);
    f32 max_x = FLOOR(aabb->max.x);
    f32 max_y = FLOOR(aabb->max.y);
    f32 max_z = aabb->max.z;
    u32 i = 0;
    Block* cb = NULL;
    for (f32 x = min_x; x <= max_x; x++)
    {
        for (f32 y = min_y; y <= max_y; y++)
        {
            for (f32 z = min_z; z <= max_z; z++)
            {
                cb = world_get_block_at(world, vec3_new(x, y, z));
                if (cb == NULL)
                    continue;
                
                BlockTouchingResult* res = &result_arr[i++];
                res->block = cb;
                res->box = (AxisAlignedBoundingBox) {
                    .min = vec3_new(x, y, z),
                    .max = vec3_new(x+1, y+1, z+1)
                };

                if (i >= result_arr_size)
                    return i;
            }
        }
    }
    return i;
}

Block* world_get_block_at(
    World* world, 
    vec3 pos
)
{
    ChunkCoord cc = world_pos_to_chunk_coord(pos);
    Chunk* chunk = world_get_or_create_chunk(world, cc);
    if (chunk == NULL)
    {
        return NULL;
    }
    
    u8 rel_x = pos.x - cc.x * CHUNK_SIZE_XZ;
    u8 rel_y = pos.y - cc.y * CHUNK_SIZE_Y;
    u8 rel_z = pos.z - cc.z * CHUNK_SIZE_XZ;
    return chunk_get_block(chunk, rel_x, rel_y, rel_z);
}

void world_tick(World* world)
{
    LOG_INFO("something tick world");
}