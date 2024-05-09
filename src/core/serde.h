#pragma once

#include "core.h"

// what im thinking:
/*
save in .gdf file with format:
1=3
2=2
3=6
a=b where
a is type GDF_MKEY
b is some other integral enum type or some float idk 

*/
typedef enum GDF_MKEY {
    GDF_MKEY_SETTINGS_DEV_CAN_FLY,
    GDF_MKEY_SETTINGS_DEV_NOCLIP,
    GDF_MKEY_SETTINGS_DEV_DRAW_WIREFRAME,

    GDF_MKEY_MAX // is this needed?
} GDF_MKEY;

char* GDF_SerializeWorld();
