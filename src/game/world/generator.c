#include "generator.h"

Generator generator_create_default()
{
    // TODO!
    Generator gen;

    return gen;
}

bool generator_gen_chunk(
    Generator* generator, 
    World* world, 
    ChunkCoord cc,
    Chunk* out_chunk
)
{
    i32 t = 1;
    GDF_ChunkBlockCreateInfo block_info = {
        .block_x = t,
        .block_y = t,
        .block_z = t,
        .type = GDF_BLOCKTYPE_Dirt
    };

    chunk_setblock(out_chunk, &block_info);
    return true;
}