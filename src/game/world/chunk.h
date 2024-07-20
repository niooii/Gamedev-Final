#pragma once 

#include "core.h"
#include "block.h"

typedef enum GDF_DIRECTION {
    GDF_DIRECTION_Up = 0,
    GDF_DIRECTION_Down = 1,
    GDF_DIRECTION_Left = 2,
    GDF_DIRECTION_Right = 3,
    GDF_DIRECTION_Forward = 4,
    GDF_DIRECTION_Backward = 5,
} GDF_DIRECTION;

typedef struct GDF_ChunkCubeFace {
    u64 data;
} GDF_ChunkCubeFace;

typedef struct GDF_Chunk {
    GDF_ChunkBlock* blocks;

    GDF_ChunkCubeFace* faces;
} GDF_Chunk;

bool GDF_CHUNK_Create(GDF_Chunk* out_chunk);
bool GDF_CHUNK_GetBlock(
    GDF_Chunk* out_chunk, 
    u8 chunk_x, 
    u8 chunk_y, 
    u8 chunk_z, 
    GDF_ChunkBlock* out_chunk_block
);
bool GDF_CHUNK_SetBlock(
    GDF_Chunk* out_chunk, 
    GDF_ChunkBlockCreateInfo* chunk_block_info
);
void GDF_CHUNK_RecalculateFaces(GDF_Chunk* chunk);