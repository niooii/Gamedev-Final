#pragma once

#include "core.h"
#include "algos.h"
#include "game/world/chunk.h"


typedef struct GDF_WorldGenParameters {
    GDF_WORLD_GEN_ALGORITHM algorithm;
} GDF_WorldGenParameters;

typedef struct GDF_Generator {
    u32 testfield;
} GDF_Generator;

GDF_Generator GDF_WORLDGEN_CreateGenerator(GDF_WorldGenParameters* parameters);

bool GDF_WORLDGEN_GenChunk(
    GDF_Generator* generator, 
    GDF_Chunk* chunk, 
    i32 chunk_x, 
    i32 chunk_y, 
    i32 chunk_z
);