#pragma once

#include "core.h"
#include "game/world/world.h"

// these save and load methods cascade downwards
// aka save world will call save player
// which will call save inv or something etc
bool GDF_SaveWorld(GDF_World* world);
GDF_World* GDF_LoadWorld();