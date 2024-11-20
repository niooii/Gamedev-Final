#pragma once

#include "core.h"
#include "game/entities/player.h"
#include "chunk.h"
#include "engine/physics/physics.h"
#include "core/collections/hashmap.h"
#include "generator.h"
#include "world_types.h"

FORCEINLINE ChunkCoord world_pos_to_chunk_coord(vec3 pos) {
    GDF_ASSERT(pos.x < i32_MAX * CHUNK_SIZE_XZ)
    GDF_ASSERT(pos.y < i32_MAX * CHUNK_SIZE_Y)
    GDF_ASSERT(pos.z < i32_MAX * CHUNK_SIZE_XZ)
    return (ChunkCoord){
        FLOOR_DIV(pos.x, CHUNK_SIZE_XZ),
        FLOOR_DIV(pos.y, CHUNK_SIZE_Y), 
        FLOOR_DIV(pos.z, CHUNK_SIZE_XZ)
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

// Called every frame.
void world_update(World* world, f64 dt);

/* This has two behaviors:
 * If a chunk doesn't exist in RAM, it will refresh what chunks are in RAM 
 * by reading from the world's save file and try again
 * If a chunk still doesn't exist, it will create it
 */
Chunk* world_get_chunk(World* world, ChunkCoord coord);

// Called every world tick by world_update()
void world_tick(World* world);