#pragma once 

#include "core.h"
#include "block.h"
#include "core/collections/list.h"

typedef enum WORLD_DIRECTION {
    WORLD_UP = 0,
    WORLD_DOWN = 1,
    WORLD_LEFT = 2,
    WORLD_RIGHT = 3,
    WORLD_FORWARD = 4,
    WORLD_BACKWARD = 5,
} WORLD_DIRECTION;

typedef struct Chunk {
    // Heap allocated array of [MAX_CHUNK_XZ * MAX_CHUNK_Y] size for direct access.
    ChunkBlock* block_arr;

    // GDF_LIST of ChunkBlock pointers for easy iteration over existing blocks.
    ChunkBlock** block_list;

    u64* faces;

    bool pending_render_update;
} Chunk;

bool chunk_create(Chunk* out_chunk);
ChunkBlock* GDF_CHUNK_GetBlock(
    Chunk* out_chunk, 
    u8 block_x, 
    u8 block_y, 
    u8 block_z
);
bool chunk_setblock(
    Chunk* out_chunk, 
    GDF_ChunkBlockCreateInfo* chunk_block_info
);
void chunk_recalc_faces(Chunk* chunk);