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
    BlockCreateInfo bi = {
        .block_x = 0,
        .block_y = 5,
        .block_z = 0,
        .type = GDF_BLOCKTYPE_Grass
    };
    chunk_set_block(out_chunk, &bi);
    bi.block_y = 1;
    bi.block_x = 1;
    bi.block_z = 1;
    chunk_set_block(out_chunk, &bi);
    bi.block_x = 2;
    bi.block_z = 2;
    chunk_set_block(out_chunk, &bi);

    for (u8 y = 0; y < 1; y++)
    {
        for (u8 x = 0; x < CHUNK_SIZE_XZ; x++)
        {
            for (u8 z = 0; z < CHUNK_SIZE_XZ; z++)
            {
                BlockCreateInfo block_info = {
                    .block_x = x,
                    .block_y = y,
                    .block_z = z,
                    .type = y
                };

                chunk_set_block(out_chunk, &block_info);
            }
        }
    }

    
    return true;
}