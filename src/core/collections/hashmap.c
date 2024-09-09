#include "hashmap.h"

GDF_HashMap __hashmap_create(u32 k_stride, u32 v_stride, bool string_keys)
{
    GDF_HashMap map = GDF_Malloc(sizeof(GDF_HashMap_T), GDF_MEMTAG_APPLICATION);
    map->k_stride = k_stride;
    map->v_stride = v_stride;
    map->string_keys = string_keys;
    map->num_entries = 0;
    map->capacity = 32;

    map->bucket = GDF_Malloc(sizeof(KeyValPair) * map->capacity, GDF_MEMTAG_APPLICATION);

    return map;
}

void GDF_HashmapInsert(GDF_HashMap hashmap, void* key, void* value)
{
    // TODO! when num entries is almost (0.75 or 0.5)x at capacity, alloc new array
    // and rehash all entries.    
}

void* GDF_HashmapGet(GDF_HashMap hashmap, void* key)
{
    return NULL;
}

void GDF_HashmapRemove(GDF_HashMap hashmap, void* key)
{
    
}