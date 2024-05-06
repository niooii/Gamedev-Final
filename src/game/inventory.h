#pragma once

#include "def.h"
#include "item.h"

typedef struct GDF_Inventory {
    GDF_Item* items[];
} GDF_Inventory; 

GDF_Inventory* GDF_MakeInventory();
