#pragma once

#include "def.h"
#include "settings/settings.h"

typedef struct Game {
    Settings* settings;
    
} Game;

bool GDF_InitGame();
bool GDF_GameUpdate();
bool GDF_GameRender();