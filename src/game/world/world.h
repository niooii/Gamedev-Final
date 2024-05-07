#pragma once

#include "core.h"
#include "game/entities/player.h"

typedef struct GDF_World {
    bool is_multiplayer;
    GDF_Player* players[];
    // Chunk* something something
} GDF_World;