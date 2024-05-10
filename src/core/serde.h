#pragma once

#include "core.h"
#include "core/d_structs/map.h"
// what im thinking:
/*
save in .gdf file with format:
CAN_FLY=3
NOCLIP=2
DRAW_WIREFRAME=6
a=b where
a is type GDF_MKEY converted into a string with GDF_MKEY_ToString(...);
b is of type GDF_MapEntry

*/

/* TODO! this is the hard part :sob: */

// serializes into format .gdf
bool GDF_SerializeMap(GDF_Map* map, char* out_buf);
// deserializes from format .gdf
bool GDF_DeserializeMap(char* data, GDF_Map* out_map);