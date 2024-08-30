#pragma once

#include "core.h"
#include "game/entities/player.h"
#include "chunk.h"
#include "worldgen/generator.h"

typedef struct World {
    // Terrain stuff
    Generator generator;
    // GDF_List
    Chunk* chunks;
    u8 chunk_simulate_distance;
    u16 ticks_per_sec;
    GDF_Stopwatch* world_update_stopwatch;
} World;

typedef struct WorldCreateInfo {
    u8 chunk_simulate_distance;
    u16 ticks_per_sec;
} WorldCreateInfo;

void world_create(World* out_world, WorldCreateInfo* create_info);

void world_tick(World* world);