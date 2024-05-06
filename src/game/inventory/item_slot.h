#pragma once

#include "def.h"
#include "game/items/item.h"

typedef struct GDF_ItemSlot {
    GDF_Item* item;
    bool has_item;
} GDF_ItemSlot;