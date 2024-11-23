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
    GDF_ChunkBlockCreateInfo bi = {
        .block_x = 0,
        .block_y = 5,
        .block_z = 0,
        .type = GDF_BLOCKTYPE_Grass
    };
    chunk_setblock(out_chunk, &bi);
    for (u8 y = 0; y < 1; y++)
    {
        for (u8 x = 0; x < CHUNK_SIZE_XZ; x++)
        {
            for (u8 z = 0; z < CHUNK_SIZE_XZ; z++)
            {
                GDF_ChunkBlockCreateInfo block_info = {
                    .block_x = x,
                    .block_y = y,
                    .block_z = z,
                    .type = y
                };

                chunk_setblock(out_chunk, &block_info);
            }
        }
    }

    
    return true;
}