#pragma once

#include "def.h"
#include "movement.h"
#include "keybinds.h"
#include "graphics.h"

typedef struct Settings {
    f32 sensitivity;
    Keybinds keybinds;
    MovementSettings movement;
    GraphicSettings graphics;
} Settings;

// search for settings.gdf or something like taht idk
Settings* GDF_LoadSettings();