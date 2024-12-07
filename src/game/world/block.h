#pragma once

#include "core.h"
#include "engine/render/textures.h"

#define CHUNK_SIZE_XZ 16
#define CHUNK_SIZE_Y 32

typedef enum BLOCKTYPE {
    GDF_BLOCKTYPE_Stone,
    GDF_BLOCKTYPE_Dirt,
    GDF_BLOCKTYPE_Grass,
    GDF_BLOCKTYPE_Glass,
} BLOCKTYPE;

typedef struct BlockTextureIds {
    u32 top;
    u32 bottom;
    u32 left;
    u32 right;
    u32 front;
    u32 back;
} BlockTextureIds;

typedef struct StaticBlockLookupData {
    BlockTextureIds tex_ids;
} StaticBlockLookupData;

const extern StaticBlockLookupData STATIC_BLOCK_LOOKUP_TABLE[];
const extern u32 STATIC_BLOCK_LOOKUP_TABLE_SIZE;

typedef struct BlockData {
    BLOCKTYPE type;
} BlockData;

typedef struct BlockCreateInfoChunk {
    BLOCKTYPE type;
    RelBlockCoord block_coord
} BlockCreateInfoChunk;

typedef struct BlockCreateInfo {
    BLOCKTYPE type;
    vec3 world_pos;
} BlockCreateInfo;

typedef struct Block {
    BlockData data;
    u8 x_rel;
    u8 y_rel;
    u8 z_rel;
    bool exists;
} Block;

typedef enum BLOCK_FACE {
    // Positive Y
    BLOCK_FACE_TOP,
    // Negative Y
    BLOCK_FACE_BOT,
    // Positive X
    BLOCK_FACE_LEFT,
    // Negative X
    BLOCK_FACE_RIGHT,
    // Positive Z
    BLOCK_FACE_FRONT,
    // Negative Z
    BLOCK_FACE_BACK,
} BLOCK_FACE;

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