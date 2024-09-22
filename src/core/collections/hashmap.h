#pragma once

#include "core.h"

typedef struct GDF_HashMap_T* GDF_HashMap;

typedef struct HashMapEntry {
    void* key;
    void* val;
    GDF_HashMap owner;
} HashMapEntry;

GDF_HashMap __hashmap_create(u32 k_stride, u32 v_stride, bool string_keys);
#define GDF_HashmapCreate(k_type, v_type, string_keys) \
    __hashmap_create(sizeof(k_type), sizeof(v_type), string_keys)

// The key and value are memcpy'd on insertion.
// Returns true on success, and false on duplicate key.
// Returns false if the requested key is NULL. 
bool GDF_HashmapInsert(GDF_HashMap hashmap, void* key, void* value);
// Gets the value stored for a key.
// Returns NULL if the key does not exist, or the requested key is NULL.
void* GDF_HashmapGet(GDF_HashMap hashmap, void* key);
// Removes the requested key from the hashmap. 
// out_val_p must be a valid pointer, or may be NULL if the removed value is to be ignored.
// Additionally, out_val_p will be set to NULL if no element exists.
void GDF_HashmapRemove(GDF_HashMap hashmap, void* key, void* out_val_p);
// Returns a pointer to the first element in the hashmap (unordered).
// Advance with GDF_HashmapIterNext. Functions like a linked list.
// returns NULL if the map is empty.
// Modifying a key will result in undefined behavior.
HashMapEntry* GDF_HashmapIter(GDF_HashMap hashmap);
// Advances the "iterator" by finding the next element in the hashmap.
// Returns true on a successful advance, returns false on end.
// Similarly sets curr to NULL on end.
// If a hashmap is modified while an iterator is still being used, behavior is undefined.
bool GDF_HashmapIterNext(HashMapEntry** iter);