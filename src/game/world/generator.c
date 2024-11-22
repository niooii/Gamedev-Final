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
    for (u8 x = 0; x < 5; x++)
    {
        GDF_ChunkBlockCreateInfo block_info = {
            .block_x = x,
            .block_y = 0,
            .block_z = 0,
            .type = x % 3
        };
        chunk_setblock(out_chunk, &block_info);
    }

    
    return true;
}