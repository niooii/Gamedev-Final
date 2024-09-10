#include "hashmap.h"
#include "superfasthash.h"

typedef struct GDF_HashMap_T {
    u32 k_stride;
    u32 v_stride;
    // TODO! string keys should have a different hash function i think
    bool string_keys;
    u32 num_entries;
    u32 capacity;
    HashMapEntry* bucket;
} GDF_HashMap_T;

static uint32_t SuperFastHash (const char * data, int len) {
uint32_t hash = (uint32_t)len, tmp;
int rem;
    if (len <= 0 || data == NULL) return 0;
    rem = len & 3;
    len >>= 2;
    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (uint32_t)(get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }
    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= (uint32_t)(signed char)data[sizeof (uint16_t)] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += (uint32_t)((signed char)*data);
                hash ^= hash << 10;
                hash += hash >> 1;
    }
    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
    return hash;
}

static FORCEINLINE u32 __get_idx(GDF_HashMap hashmap, void* key)
{
    return SuperFastHash((const char*)key, hashmap->k_stride) % hashmap->capacity;
}

GDF_HashMap __hashmap_create(u32 k_stride, u32 v_stride, bool string_keys)
{
    GDF_HashMap map = GDF_Malloc(sizeof(GDF_HashMap_T), GDF_MEMTAG_APPLICATION);
    map->k_stride = k_stride;
    map->v_stride = v_stride;
    map->string_keys = string_keys;
    map->num_entries = 0;
    map->capacity = 32;

    map->bucket = GDF_Malloc(sizeof(HashMapEntry) * map->capacity, GDF_MEMTAG_APPLICATION);

    return map;
}

bool GDF_HashmapInsert(GDF_HashMap hashmap, void* key, void* value)
{
    if (key == NULL)
        return false;
    // TODO! when num entries is almost (0.75 or 0.5)x at capacity, alloc new array
    // and rehash all entries.

    // Check if map needs to be expanded
    if (hashmap->num_entries + 1 >= (hashmap->capacity / 2))
    {
        // TODO! Expand map
        // if ()
    }

    HashMapEntry* bucket = hashmap->bucket;
    // Find next free index to insert in
    u32 start_idx = __get_idx(hashmap, key);
    u32 idx;
    for (u32 i = 0; i < hashmap->capacity && bucket[(
        (idx = (start_idx + i) % hashmap->capacity) 
    )].key != NULL; i++)
    {
        // Check if we have a duplicate key.
        if (memcmp(bucket[idx].key, key, hashmap->k_stride) == 0)
            return false;
    }
    
    bucket[idx].owner = hashmap;
    bucket[idx].key = GDF_Malloc(hashmap->k_stride, GDF_MEMTAG_APPLICATION);
    GDF_MemCopy(bucket[idx].key, key, hashmap->k_stride);
    bucket[idx].val = GDF_Malloc(hashmap->v_stride, GDF_MEMTAG_APPLICATION);
    GDF_MemCopy(bucket[idx].val, value, hashmap->v_stride);

    return true;
}

void* GDF_HashmapGet(GDF_HashMap hashmap, void* key)
{
    if (key == NULL)
        return NULL;
    
    HashMapEntry* bucket = hashmap->bucket;
    u32 start_idx = __get_idx(hashmap, key);

    // Linear probing, wrap around until something is found or nothing is found.
    for (u32 i = 0, idx; i < hashmap->capacity && bucket[(
        (idx = (start_idx + i) % hashmap->capacity) 
    )].key != NULL; i++)
    {
        if (memcmp(bucket[idx].key, key, hashmap->k_stride) == 0)
        {
            return bucket[idx].val;
        } 
    }

    return NULL;
}

void GDF_HashmapRemove(GDF_HashMap hashmap, void* key)
{
    if (key == NULL)
        return;

    // TODO!
}

HashMapEntry* GDF_HashmapIter(GDF_HashMap hashmap)
{
    if (hashmap->num_entries == 0) 
        return NULL;

    for (int i = 0; i < hashmap->capacity; i++)
    {
        if (hashmap->bucket[i].key != NULL)
            return &hashmap->bucket[i];
    }

    return NULL;
}

bool GDF_HashmapIterNext(HashMapEntry** iter)
{
    GDF_HashMap hashmap = (*iter)->owner;

    u32 idx = ((*iter) - hashmap->bucket) + 1;
    HashMapEntry* bucket = hashmap->bucket;
    for (; idx < hashmap->capacity; idx++)
    {
        if (bucket[idx].key != NULL)
        {
            *iter = &bucket[idx];
            return true;
        }
    }

    *iter = NULL;
    return false;
}