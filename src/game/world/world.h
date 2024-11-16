#pragma once

#include "core.h"
#include "game/entities/player.h"
#include "chunk.h"
#include "worldgen/generator.h"
#include "engine/physics/physics.h"
#include "core/collections/hashmap.h"

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

    PhysicsEngine physics;
} World;

typedef struct WorldCreateInfo {
    u8 chunk_simulate_distance;
    u16 ticks_per_sec;
} WorldCreateInfo;

void world_create(World* out_world, WorldCreateInfo* create_info);

// Called every frame.
void world_update(World* world, f64 dt);

// Called every world tick by world_update()
void world_tick(World* world);