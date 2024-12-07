#include "chunk.h"

bool chunk_init(Chunk* out_chunk)
{
    out_chunk->block_arr = GDF_Malloc(CHUNK_SIZE_XZ * CHUNK_SIZE_XZ * CHUNK_SIZE_Y * sizeof(Block), GDF_MEMTAG_GAME);
    out_chunk->block_list = GDF_LIST_Reserve(Block*, 2048);

    out_chunk->faces = NULL;
    out_chunk->pending_render_update = true;
    return true;
}

static FORCEINLINE i32 __to_idx(
    RelBlockCoord bc
)
{
    return bc.block_x + (bc.block_y * CHUNK_SIZE_XZ) + (bc.block_z * CHUNK_SIZE_XZ * CHUNK_SIZE_Y);
}

Block* chunk_get_block(
    Chunk* chunk, 
    RelBlockCoord bc
)
{
    Block* chunk_block = &chunk->block_arr[
        __to_idx(
            bc
        )
    ];
    
    if (!chunk_block->exists)
        return NULL;
    else
        return chunk_block;
}

bool chunk_set_block(
    Chunk* chunk, 
    BlockCreateInfoChunk* chunk_block_info
)
{
    Block* block = &chunk->block_arr[
        __to_idx(
            chunk_block_info->block_coord
        )
    ];

    block->exists = true;
    block->x_rel = chunk_block_info->block_coord.block_x;
    block->y_rel = chunk_block_info->block_coord.block_y;
    block->z_rel = chunk_block_info->block_coord.block_z;

    block->data.type = chunk_block_info->type;

    GDF_LIST_Push(chunk->block_list, block);

    return true;
}

void chunk_destroy_block(
    Chunk* chunk, 
    RelBlockCoord bc,
    Block* out
)
{
    Block* block = &chunk->block_arr[
        __to_idx(
            bc
        )
    ];
    block->exists = false;
    // TODO! THIS IS HORRIBLE USE A HASHMAP
    
}

void chunk_recalc_faces(Chunk* chunk)
{
    TODO("recalc chunk faces");
} 