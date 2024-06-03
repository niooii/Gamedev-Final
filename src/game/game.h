#pragma once

#include "core.h"
#include "world/world.h"

typedef struct GDF_Game {
    World* world;
} GDF_Game;

bool GDF_InitGame();
bool GDF_GameUpdate();
bool GDF_GameRender();
GDF_Game* GDF_GetGame();