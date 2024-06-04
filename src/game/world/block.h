#pragma once
#include "core.h"

typedef enum BLOCK_ID {
    BLOCK_ID_AIR,
    BLOCK_ID_DIRT,
    BLOCK_ID_GRASS,
} BLOCK_ID;

// TODO! texture natural lighting and other stuff
typedef struct BlockInfo {
    BLOCK_ID id;
    const char* name;
    f32 strength;
    bool interactable;
} BlockInfo;

BlockInfo* get_block_info(BLOCK_ID id);