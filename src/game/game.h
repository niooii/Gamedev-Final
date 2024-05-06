#pragma once

#include "def.h"
#include "settings/settings.h"

typedef struct GDF_Game {
    GDF_Settings* settings;
    
} GDF_Game;

bool GDF_InitGame();
bool GDF_GameUpdate();
bool GDF_GameRender();