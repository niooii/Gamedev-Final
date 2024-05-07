#pragma once

#include "core.h"
#include "settings/settings.h"
#include "world/world.h"

typedef struct GDF_Game {
    GDF_Settings* settings;
    GDF_World* world;
} GDF_Game;

bool GDF_InitGame();
bool GDF_GameUpdate();
bool GDF_GameRender();
GDF_Game* GDF_GetGame();