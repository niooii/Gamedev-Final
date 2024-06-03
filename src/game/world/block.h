#pragma once
#include "core.h"

typedef enum BLOCK_ID {
    BLOCK_ID_AIR,
    BLOCK_ID_DIRT,
    BLOCK_ID_GRASS,
} BLOCK_ID;

typedef struct BlockInfo {
    BLOCK_ID id;
    const char* name;
    f32 strength;
    bool interactable;
} BlockInfo;

bool InitBlocks();