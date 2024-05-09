#pragma once

#include "core.h"
#include "game/entities/player.h"

typedef struct GDF_World {
    const char* world_name;
    bool is_multiplayer;
    GDF_Player* players[];
    // Chunk* something something
} GDF_World;

typedef struct GDF_WorldCreateOptions {
    const char* name;
} GDF_WorldCreateOptions;

GDF_World* GDF_CreateWorld(GDF_WorldCreateOptions* options);
void GDF_FreeWorld();