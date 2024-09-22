#pragma once

#include "core.h"
#include "key.h"
#include <string.h>
#include <stdio.h>

/*
ATTENTION ACTUAL GOOD PROGRAMMERS:
this is not a real map. this is a
rushed one-purpose map implementation
solely for providing a common serialization interface.
*/
// needed for serialization

typedef enum GDF_MAP_DTYPE {
    GDF_MAP_DTYPE_INT,
    GDF_MAP_DTYPE_BOOL,
    GDF_MAP_DTYPE_DOUBLE,
    GDF_MAP_DTYPE_STRING,
    // HEHEHEAW  
    GDF_MAP_DTYPE_MAP,
    // TODO!
    GDF_MAP_DTYPE_ARRAY, 
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

bool* GDF_MAP_GetValueBool(GDF_Map* map, GDF_MKEY key);
f64* GDF_MAP_GetValuef64(GDF_Map* map, GDF_MKEY key);
i32* GDF_MAP_GetValuei32(GDF_Map* map, GDF_MKEY key);
const char* GDF_MAP_GetValueString(GDF_Map* map, GDF_MKEY key);
GDF_Map* GDF_MAP_GetValueMap(GDF_Map* map, GDF_MKEY key);

void GDF_FreeMap(GDF_Map* map);