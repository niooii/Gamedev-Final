#pragma once

#include "core.h"
#include "engine/render/textures.h"

#define MAX_CHUNK_XZ 16
#define MAX_CHUNK_Y 32

typedef enum BLOCKTYPE {
    GDF_BLOCKTYPE_AIR,
    GDF_BLOCKTYPE_Grass,
    GDF_BLOCKTYPE_Dirt,
    GDF_BLOCKTYPE_Glass,
    GDF_BLOCKTYPE_Stone,
} BLOCKTYPE;

typedef struct BlockTextureIds {
    u8 top;
    u8 bottom;
    u8 left;
    u8 right;
    u8 front;
    u8 back;
} BlockTextureIds;

const extern BlockTextureIds block_texture_ids[];

typedef struct BlockData {
    BLOCKTYPE type;
} BlockData;

typedef struct GDF_ChunkBlockCreateInfo {
    BLOCKTYPE type;
    u8 block_x;
    u8 block_y;
    u8 block_z;
} GDF_ChunkBlockCreateInfo;

typedef struct ChunkBlock {
    BlockData data;
    u8 chunk_x;
    u8 chunk_y;
    u8 chunk_z;
    bool exists;
} ChunkBlock;

typedef enum GDF_BLOCK_OFFSETS {
    GDF_BLOCK_OFFSETS_FaceDirection = 0,
    GDF_BLOCK_OFFSETS_FaceChunkPosX = 3,
    GDF_BLOCK_OFFSETS_FaceChunkPosY = 9,
    GDF_BLOCK_OFFSETS_FaceChunkPosZ = 15,
    GDF_BLOCK_OFFSETS_FaceTextureId = 21,
} GDF_BLOCK_OFFSETS;

typedef enum GDF_BLOCK_BITS {
    GDF_BLOCK_BITS_FaceDirection = 3,
    GDF_BLOCK_BITS_FaceChunkPosX = 6,
    GDF_BLOCK_BITS_FaceChunkPosY = 6,
    GDF_BLOCK_BITS_FaceChunkPosZ = 6,
    GDF_BLOCK_BITS_FaceTextureId = 8,
} GDF_BLOCK_BITS;