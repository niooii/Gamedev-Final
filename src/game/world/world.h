#pragma once

#include "core.h"
#include "game/entities/player.h"
#include "chunk.h"

typedef struct GDF_World {
    // GDF_List
    GDF_Chunk* chunks;
} GDF_World;