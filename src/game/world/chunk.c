#include "chunk.h"

bool GDF_CHUNK_Create(GDF_Chunk* out_chunk)
{
    return false;
}

bool GDF_CHUNK_GetBlock(
    GDF_Chunk* chunk, 
    u8 chunk_x, 
    u8 chunk_y, 
    u8 chunk_z, 
    GDF_ChunkBlock* out_chunk_block
)
{
    return false;
}

bool GDF_CHUNK_SetBlock(
    GDF_Chunk* chunk, 
    GDF_ChunkBlockCreateInfo* chunk_block_info
)
{
    return false;
}

void GDF_CHUNK_RecalculateFaces(GDF_Chunk* chunk)
{
    
}