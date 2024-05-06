#pragma once

#include "def.h"
#include "movement.h"
#include "keybinds.h"
#include "graphics.h"
#include "mouse.h"

typedef struct Settings {
    Keybinds keybinds;
    MouseSettings mouse;
    MovementSettings movement;
    GraphicSettings graphics;
} Settings;

// search for settings.gdf or something like taht idk
Settings* GDF_LoadSettings();
void GDF_SaveSettings();