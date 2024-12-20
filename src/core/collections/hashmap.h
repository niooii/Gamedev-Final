#pragma once

#include "core.h"
#include "superfasthash.h"

static FORCEINLINE u32 superfasthash_wrapper(const u8* data, u32 len) 
{
    return (u32)SuperFastHash((const char*)data, (int) len);
}

typedef struct GDF_HashMap_T* GDF_HashMap;

#define GDF_HashMap(key, type) GDF_HashMap

typedef struct HashmapEntry {
    void* key;
    void* val;
    GDF_HashMap owner;
} HashmapEntry;

GDF_HashMap __hashmap_create(u32 k_stride, u32 v_stride, u32 (*hash_func)(const u8* data, u32 len), bool string_keys);

#define GDF_HashmapCreate(k_type, v_type, string_keys) \
    __hashmap_create(sizeof(k_type), sizeof(v_type), NULL, string_keys)

#define GDF_HashmapWithHasher(k_type, v_type, hash_func, string_keys) \
    __hashmap_create(sizeof(k_type), sizeof(v_type), hash_func, string_keys)

bool GDF_HashmapDestroy(GDF_HashMap hashmap);

// The key and value are memcpy'd on insertion.
// Returns a pointer to the inserted value (owned by hashmap) on success, and NULL on duplicate key.
// Returns NULL if the requested key is NULL. 
void* GDF_HashmapInsert(GDF_HashMap hashmap, void* key, void* value);
// Gets the value stored for a key.
// Returns NULL if the key does not exist, or the requested key is NULL.
void* GDF_HashmapGet(GDF_HashMap hashmap, void* key);
// Removes the requested key from the hashmap. 
// out_val_p must be a valid pointer, or may be NULL if the removed value is to be ignored.
// Additionally, out_val_p will be set to NULL if no element exists.
void GDF_HashmapRemove(GDF_HashMap hashmap, void* key, void* out_val_p);
// The amount of elements present in the hashmap.
u32 GDF_HashmapLen(GDF_HashMap hashmap);
// Returns a pointer to the first element in the hashmap (unordered).
// Advance with GDF_HashmapIterNext. Functions like a linked list.
// returns NULL if the map is empty.
// Modifying a key will result in undefined behavior.
HashmapEntry* GDF_HashmapIter(GDF_HashMap hashmap);
// Advances the "iterator" by finding the next element in the hashmap.
// Returns true on a successful advance, returns false on end.
// Similarly sets curr to NULL on end.
// If a hashmap is modified while an iterator is still being used, behavior is undefined.
bool GDF_HashmapIterAdvance(HashmapEntry** iter);