#pragma once

#include "core.h"
#include <string.h>
#include <stdio.h>

/*
ATTENTION ACTUAL GOOD PROGRAMMERS:
this is not a real map. this is a
rushed one-purpose map implementation
solely for providing a common serialization interface.
*/
// needed for serialization
void GDF_MKEY_InitLookupTable();

typedef enum GDF_MKEY {
    GDF_MKEY_SETTINGS_DEV_CAN_FLY,
    GDF_MKEY_SETTINGS_DEV_NOCLIP,
    GDF_MKEY_SETTINGS_DEV_DRAW_WIREFRAME,

    // not actual keys anymore
    GDF_MKEY_NUM_KEYS, // is this needed?
    GDF_MKEY_ERROR_KEY // this shouldnt be used at all to index anything
} GDF_MKEY;
typedef struct { char* str; GDF_MKEY key; } t_symstruct;
// returns NULL on failure
void GDF_MKEY_ToString(GDF_MKEY key, char* out_str);
// returns GDF_MKEY_ERROR_KEY on failure
GDF_MKEY GDF_MKEY_FromString(const char* str);

typedef enum GDF_MAP_DTYPE {
    GDF_MAP_DTYPE_INT,
    GDF_MAP_DTYPE_BOOL,
    GDF_MAP_DTYPE_DOUBLE,
    GDF_MAP_DTYPE_STRING,
    // HEHEHEAW  
    GDF_MAP_DTYPE_MAP,
} GDF_MAP_DTYPE;

typedef struct GDF_MapEntry {
    void* value;
    GDF_MAP_DTYPE dtype;
} GDF_MapEntry;

// heap allocated horrific map implementation
typedef struct GDF_Map {
    GDF_MapEntry* entries[GDF_MKEY_NUM_KEYS];
} GDF_Map;

// allocates a map pointer
GDF_Map* GDF_CreateMap();

// void* value is copied and allocated on the heap.
bool GDF_AddMapEntry(GDF_Map* map, GDF_MKEY key, void* value, GDF_MAP_DTYPE dtype);

// should return null if none
GDF_MapEntry* GDF_GetMapEntry(GDF_Map* map, GDF_MKEY key);

void GDF_FreeMap(GDF_Map* map);