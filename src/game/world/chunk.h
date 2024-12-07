#pragma once 

#include "core.h"
#include "block.h"
#include "core/collections/list.h"
#include "world_types.h"

typedef struct RelBlockCoord {
    u8 block_x; 
    u8 block_y; 
    u8 block_z;
} RelBlockCoord;

bool chunk_init(Chunk* out_chunk);
Block* chunk_get_block(
    Chunk* out_chunk, 
    RelBlockCoord block_coord
);
bool chunk_set_block(
    Chunk* out_chunk, 
    BlockCreateInfoChunk* chunk_block_info
);
void chunk_destroy_block(
    Chunk* chunk, 
    RelBlockCoord block_coord,
    Block* out
);
void chunk_recalc_faces(Chunk* chunk);