#pragma once

#include "core.h"
#include "engine/render/textures.h"

#define MAX_CHUNK_XZ 16
#define MAX_CHUNK_Y 32

typedef enum GDF_BLOCKTYPE {
    GDF_BLOCKTYPE_Grass,
    GDF_BLOCKTYPE_Dirt,
    GDF_BLOCKTYPE_Glass,
} GDF_BLOCKTYPE;

typedef struct GDF_CubeTextureIds {
    u8 top;
    u8 bottom;
    u8 left;
    u8 right;
    u8 front;
    u8 back;
} GDF_CubeTextureIds;

bool FORCEINLINE GDF_GetDefaultCubeTextures(GDF_BLOCKTYPE block_type, GDF_CubeTextureIds* tex_ids)
{
    u8 top;
    u8 bottom;
    u8 left;
    u8 right;
    u8 front;
    u8 back;
    switch (block_type)
    {
        case GDF_BLOCKTYPE_Grass:
        {
            top = GDF_TEXTURE_INDEX_GRASS_TOP;
            bottom = GDF_TEXTURE_INDEX_DIRT;
            left = GDF_TEXTURE_INDEX_GRASS_SIDE;
            right = GDF_TEXTURE_INDEX_GRASS_SIDE;
            front = GDF_TEXTURE_INDEX_GRASS_SIDE;
            back = GDF_TEXTURE_INDEX_GRASS_SIDE;
            break;
        }
        case GDF_BLOCKTYPE_Dirt:
        {
            top = GDF_TEXTURE_INDEX_DIRT;
            bottom = GDF_TEXTURE_INDEX_DIRT;
            left = GDF_TEXTURE_INDEX_DIRT;
            right = GDF_TEXTURE_INDEX_DIRT;
            front = GDF_TEXTURE_INDEX_DIRT;
            back = GDF_TEXTURE_INDEX_DIRT;
            break;
        }

        default: 
        {
            return false;
        }
    }

    tex_ids->top = top;
    tex_ids->bottom = bottom;
    tex_ids->left = left;
    tex_ids->right = right;
    tex_ids->front = front;
    tex_ids->back = back;

    return true;
}

typedef struct GDF_ChunkBlockCreateInfo {
    GDF_BLOCKTYPE type;
    u8 chunk_x;
    u8 chunk_y;
    u8 chunk_z;
} GDF_ChunkBlockCreateInfo;

// These get directly inserted to chunks, no need to store position data
typedef struct GDF_ChunkBlock {
    GDF_CubeTextureIds cube_textures;
} GDF_ChunkBlock;

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