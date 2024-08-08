#include "generator.h"

GDF_Generator GDF_WORLDGEN_CreateGenerator(GDF_WorldGenParameters* parameters)
{
    // TODO!
    GDF_Generator gen;

    return gen;
}

bool GDF_WORLDGEN_GenChunk(
    GDF_Generator* generator, 
    GDF_Chunk* chunk, 
    i32 chunk_x, 
    i32 chunk_y, 
    i32 chunk_z
)
{
    i32 t = 1;
    GDF_ChunkBlockCreateInfo block_info = {
        .block_x = t,
        .block_y = t,
        .block_z = t,
        .type = GDF_BLOCKTYPE_Dirt
    };

    GDF_CHUNK_SetBlock(chunk, &block_info);
    return true;
}