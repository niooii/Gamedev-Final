#pragma once

#include "core.h"
#include "superfasthash.h"

typedef struct KeyValPair {
    void* key;
    void* val;
} KeyValPair;

typedef struct GDF_HashMap_T {
    u32 k_stride;
    u32 v_stride;
    // TODO! string keys should have a different hash function i think
    bool string_keys;
    u32 num_entries;
    u32 capacity;
    KeyValPair* bucket;
} GDF_HashMap_T;

typedef struct GDF_HashMap_T* GDF_HashMap;

GDF_HashMap __hashmap_create(u32 k_stride, u32 v_stride, bool string_keys);
// The key and value are memcpy'd on insertion. 
#define GDF_HashmapCreate(k_type, v_type, string_keys) \
    __hashmap_create(sizeof(k_type), sizeof(v_type), string_keys)

void GDF_HashmapInsert(GDF_HashMap hashmap, void* key, void* value);
// Returns NULL if the key does not exist.
void* GDF_HashmapGet(GDF_HashMap hashmap, void* key);
void GDF_HashmapRemove(GDF_HashMap hashmap, void* key);