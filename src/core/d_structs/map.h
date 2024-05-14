#pragma once

#include "core.h"

/*
ATTENTION ACTUAL GOOD PROGRAMMERS:
this is not a real map. this is a
rushed one-purpose map implementation
solely for providing a common serialization interface.
*/

typedef enum GDF_MKEY {
    GDF_MKEY_SETTINGS_DEV_CAN_FLY,
    GDF_MKEY_SETTINGS_DEV_NOCLIP,
    GDF_MKEY_SETTINGS_DEV_DRAW_WIREFRAME,

    // not actual keys anymore
    GDF_MKEY_NUM_KEYS, // is this needed?
    GDF_MKEY_ERROR_KEY // this shouldnt be used at all to index anything
} GDF_MKEY;


// returns NULL on failure
void GDF_MKEY_ToString(GDF_MKEY key, char* out_str);
// returns GDF_MKEY_ERROR_KEY on failure
void GDF_MKEY_FromString(const char* str, GDF_MKEY* out_key);

typedef enum GDF_MAP_DTYPE {
    GDF_MAP_DTYPE_INT,
    GDF_MAP_DTYPE_FLOAT,
    GDF_MAP_DTYPE_STRING,
    // HEHEHEAW
    GDF_MAP_DTYPE_MAP,
} GDF_MAP_DTYPE;

// heap allocated horrific map implementation
typedef struct GDF_Map {
    GDF_MapEntry entries[1];
} GDF_Map;

typedef struct GDF_MapEntry {
    GDF_MKEY key;
    void* value;
    GDF_MAP_DTYPE dtype;
} GDF_MapEntry;

// allocates a map pointer
GDF_Map* GDF_CreateMap();

// should return null if none
GDF_MapEntry* GDF_GetMapEntry(GDF_MKEY key);

void GDF_FreeMap(GDF_Map* map);