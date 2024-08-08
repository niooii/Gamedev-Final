#pragma once

#include "core.h"
#include "game/entities/player.h"
#include "chunk.h"
#include "worldgen/generator.h"


typedef struct GDF_World {
    // Terrain stuff
    GDF_Generator generator;
    // GDF_List
    GDF_Chunk* chunks;
    u8 chunk_simulate_distance;
    u16 ticks_per_sec;
    GDF_Stopwatch* since_last_tick;
} GDF_World;

typedef struct GDF_WorldCreateInfo {
    u8 chunk_simulate_distance;
    u16 ticks_per_sec;
} GDF_WorldCreateInfo;

void GDF_WORLD_Create(GDF_World* out_world, GDF_WorldCreateInfo* create_info);

void GDF_WORLD_Tick(GDF_World* world);