#pragma once

#include "def.h"
#include "game/entities/player.h"

typedef struct GDF_World {
    GDF_Player* players[];
} GDF_World;