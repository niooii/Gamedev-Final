#pragma once

#include "core.h"
#include "game/entities/player.h"

typedef struct World {
    const char* world_name;
    bool is_multiplayer;
    GDF_Player* players[];
    // Chunk* something something
} World;

typedef struct WorldCreateOptions {
    const char* name;
} WorldCreateOptions;

World* GDF_CreateWorld(WorldCreateOptions* options);
void GDF_FreeWorld();