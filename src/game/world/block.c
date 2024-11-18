#include "block.h"

const StaticBlockLookupData STATIC_BLOCK_LOOKUP_TABLE[] = {
    [GDF_BLOCKTYPE_Grass] = {
        .tex_ids = {
            .top = GDF_TEXTURE_INDEX_GRASS_TOP,
            .bottom = GDF_TEXTURE_INDEX_DIRT,
            .left = GDF_TEXTURE_INDEX_GRASS_SIDE,
            .right = GDF_TEXTURE_INDEX_GRASS_SIDE,
            .front = GDF_TEXTURE_INDEX_GRASS_SIDE,
            .back = GDF_TEXTURE_INDEX_GRASS_SIDE
        }
    },
    [GDF_BLOCKTYPE_Dirt] = {
        .tex_ids = {
            .top = GDF_TEXTURE_INDEX_DIRT,
            .bottom = GDF_TEXTURE_INDEX_DIRT,
            .left = GDF_TEXTURE_INDEX_DIRT,
            .right = GDF_TEXTURE_INDEX_DIRT,
            .front = GDF_TEXTURE_INDEX_DIRT,
            .back = GDF_TEXTURE_INDEX_DIRT
        }
    },
    [GDF_BLOCKTYPE_Stone] = {
        .tex_ids = {
            .top = GDF_TEXTURE_INDEX_STONE,
            .bottom = GDF_TEXTURE_INDEX_STONE,
            .left = GDF_TEXTURE_INDEX_STONE,
            .right = GDF_TEXTURE_INDEX_STONE,
            .front = GDF_TEXTURE_INDEX_STONE,
            .back = GDF_TEXTURE_INDEX_STONE 
        }
    },
};

const u32 STATIC_BLOCK_LOOKUP_TABLE_SIZE = sizeof(STATIC_BLOCK_LOOKUP_TABLE);