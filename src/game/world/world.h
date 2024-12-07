#pragma once

#include "core.h"
#include "game/entity/humanoid.h"
#include "chunk.h"
#include "engine/physics/physics.h"
#include "engine/physics/aabb.h"
#include "core/collections/hashmap.h"
#include "generator.h"
#include "world_types.h"

FORCEINLINE ChunkCoord world_pos_to_chunk_coord(vec3 pos) {
    // GDF_ASSERT(pos.x < i32_MAX * CHUNK_SIZE_XZ)
    // GDF_ASSERT(pos.y < i32_MAX * CHUNK_SIZE_Y)
    // GDF_ASSERT(pos.z < i32_MAX * CHUNK_SIZE_XZ)
    return (ChunkCoord){
        FLOOR(pos.x / CHUNK_SIZE_XZ),
        FLOOR(pos.y / CHUNK_SIZE_Y), 
        FLOOR(pos.z / CHUNK_SIZE_XZ)
    };
}

FORCEINLINE vec3 chunk_coord_to_world_pos(ChunkCoord coord) {
    return (vec3){
        coord.x * CHUNK_SIZE_XZ,
        coord.y * CHUNK_SIZE_Y,
        coord.z * CHUNK_SIZE_XZ
    };
}

void world_create(World* out_world, WorldCreateInfo* create_info);
// Creates a new Humanoid Entity and returns it for modification.
HumanoidEntity* world_create_humanoid(World* world);

// Called every frame.
void world_update(World* world, f64 dt);

/* This has two behaviors:
 * If a chunk doesn't exist in RAM, it will refresh what chunks are in RAM 
 * by reading from the world's save file and try again
 * If a chunk still doesn't exist, it will create it
 */
Chunk* world_get_or_create_chunk(World* world, ChunkCoord coord);

// Pass NULL for destroyed_block if you don't care 
// about the properties of the destroyed block.
void world_destroy_block(World* world, vec3 block_world_pos, Block* destroyed_block);

Block* world_set_block(World* world, BlockCreateInfo* create_info);

typedef struct BlockTouchingResult {
    Block* block;
    AxisAlignedBoundingBox box;
} BlockTouchingResult;

// Gets the blocks that is touching an AABB.
// Modifies the result_arr with the found blocks, and returns the amount of blocks found
u32 world_get_blocks_touching(
    World* world, 
    AxisAlignedBoundingBox* aabb, 
    BlockTouchingResult* result_arr,
    u32 result_arr_size
);

Block* world_get_block_at(
    World* world, 
    vec3 pos
);

// Called every world tick by world_update()
void world_tick(World* world);