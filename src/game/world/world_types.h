#pragma once

#include "core.h"
#include "core/collections/hashmap.h"
#include "core/collections/list.h"
#include "engine/physics/physics.h"
#include "game/entity/entity.h"
#include "game/entity/humanoid.h"

typedef struct Block Block;

typedef enum WORLD_DIRECTION {
    WORLD_UP = 0,
    WORLD_DOWN = 1,
    WORLD_LEFT = 2,
    WORLD_RIGHT = 3,
    WORLD_FORWARD = 4,
    WORLD_BACKWARD = 5,
} WORLD_DIRECTION;

typedef struct Chunk {
    // Heap allocated array of [CHUNK_SIZE_XZ * CHUNK_SIZE_XZ * MAX_CHUNK_Y] size for direct access.
    Block* block_arr;

    // GDF_LIST of ChunkBlock pointers for easy iteration over existing blocks.
    Block** block_list;

    u64* faces;

    bool pending_render_update;
} Chunk;

typedef struct Generator {
    u64 testfield;
} Generator;

typedef struct ChunkCoord {
    i32 x;
    i32 y;
    i32 z;
} ChunkCoord;

typedef struct World {
    // Terrain stuff
    Generator generator;
    // <ChunkCoord, Chunk>
    GDF_HashMap(ChunkCoord, Chunk) chunks;
    u8 chunk_simulate_distance;
    u16 ticks_per_sec;
    GDF_Stopwatch world_update_stopwatch;
    
    GDF_LIST(HumanoidEntity*) humanoids;
    
    PhysicsEngine physics;
} World;

typedef struct WorldCreateInfo {
    u8 chunk_simulate_distance;
    u16 ticks_per_sec;
} WorldCreateInfo;