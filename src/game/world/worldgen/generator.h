#pragma once

#include "core.h"
#include "game/world/chunk.h"

typedef struct Generator {
    u64 testfield;
} Generator;

Generator generator_create_default();

bool generator_make_chunk(
    Generator* generator, 
    Chunk* chunk, 
    i32 chunk_x, 
    i32 chunk_y, 
    i32 chunk_z
);