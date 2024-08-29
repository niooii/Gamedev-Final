#include "generator.h"

Generator generator_create(GeneratorCreateInfo* parameters)
{
    // TODO!
    Generator gen;

    return gen;
}

bool generator_make_chunk(
    Generator* generator, 
    Chunk* chunk, 
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

    chunk_setblock(chunk, &block_info);
    return true;
}