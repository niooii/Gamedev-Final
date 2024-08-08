#pragma once 

#include "core.h"
#include "block.h"
#include "engine/core/containers/list.h"

typedef enum GDF_DIRECTION {
    GDF_DIRECTION_Up = 0,
    GDF_DIRECTION_Down = 1,
    GDF_DIRECTION_Left = 2,
    GDF_DIRECTION_Right = 3,
    GDF_DIRECTION_Forward = 4,
    GDF_DIRECTION_Backward = 5,
} GDF_DIRECTION;

typedef struct GDF_Chunk {
    // Heap allocated array of [MAX_CHUNK_XZ * MAX_CHUNK_Y] size for direct access.
    GDF_ChunkBlock* block_arr;

    // GDF_LIST of ChunkBlock pointers for easy iteration over existing blocks.
    GDF_ChunkBlock** block_list;

    u64* faces;

    bool pending_render_update;
} GDF_Chunk;

bool GDF_CHUNK_Create(GDF_Chunk* out_chunk);
GDF_ChunkBlock* GDF_CHUNK_GetBlock(
    GDF_Chunk* out_chunk, 
    u8 block_x, 
    u8 block_y, 
    u8 block_z
);
bool GDF_CHUNK_SetBlock(
    GDF_Chunk* out_chunk, 
    GDF_ChunkBlockCreateInfo* chunk_block_info
);
void GDF_CHUNK_RecalculateFaces(GDF_Chunk* chunk);