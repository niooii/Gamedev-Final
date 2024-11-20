#include "chunk.h"

bool chunk_init(Chunk* out_chunk)
{
    out_chunk->block_arr = GDF_Malloc(CHUNK_SIZE_XZ * CHUNK_SIZE_XZ * CHUNK_SIZE_Y * sizeof(ChunkBlock), GDF_MEMTAG_GAME);
    out_chunk->block_list = GDF_LIST_Reserve(ChunkBlock*, 2048);

    out_chunk->faces = NULL;
    out_chunk->pending_render_update = true;
    return true;
}

ChunkBlock* chunk_getblock(
    Chunk* chunk, 
    u8 block_x, 
    u8 block_y, 
    u8 block_z
)
{
    ChunkBlock* chunk_block = &chunk->block_arr[block_x * block_y * block_z];
    
    if (!chunk_block->exists)
        return NULL;
    else
        return chunk_block;
}

bool chunk_setblock(
    Chunk* chunk, 
    GDF_ChunkBlockCreateInfo* chunk_block_info
)
{
    ChunkBlock* block = &chunk->block_arr[
        chunk_block_info->block_x * 
        chunk_block_info->block_y * 
        chunk_block_info->block_z
    ];

    block->exists = true;
    block->x_rel = chunk_block_info->block_x;
    block->y_rel = chunk_block_info->block_y;
    block->z_rel = chunk_block_info->block_z;

    block->data.type = chunk_block_info->type;
    // if (!cube_textures_get_default(block->data.type, &block->data.textures))
    // {
    //     LOG_ERR("No default textures for block type %u", block->data.type);
    //     return false;
    // }

    GDF_LIST_Push(chunk->block_list, block);

    return true;
}

void chunk_recalc_faces(Chunk* chunk)
{
    
}