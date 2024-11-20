#pragma once

#include "core.h"
#include "chunk.h"
#include "world_types.h"

Generator generator_create_default();

bool generator_gen_chunk(
    Generator* generator, 
    World* world, 
    ChunkCoord cc,
    Chunk* out_chunk
);