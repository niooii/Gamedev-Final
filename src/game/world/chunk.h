#pragma once 

#include "core.h"
#include "block.h"
#include "core/collections/list.h"
#include "world_types.h"

bool chunk_init(Chunk* out_chunk);
Block* chunk_getblock(
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