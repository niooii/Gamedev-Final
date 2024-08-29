#pragma once

#include "core.h"
#include "algos.h"
#include "game/world/chunk.h"


typedef struct GeneratorCreateInfo {
    GDF_WORLD_GEN_ALGORITHM algorithm;
} GeneratorCreateInfo;

typedef struct Generator {
    u32 testfield;
} Generator;

Generator generator_create(GeneratorCreateInfo* parameters);

bool generator_make_chunk(
    Generator* generator, 
    Chunk* chunk, 
    i32 chunk_x, 
    i32 chunk_y, 
    i32 chunk_z
);