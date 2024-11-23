#pragma once

#include "core.h"
#include "core/collections/hashmap.h"
#include "engine/physics/physics.h"
#include "game/entity/entity.h"
#include "game/entity/humanoid.h"

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