#include "hashmap.h"
#include "superfasthash.h"
    #include <stdio.h>

typedef struct GDF_HashMap_T {
    u32 k_stride;
    u32 v_stride;
    // TODO! string keys should have a different hash function i think
    bool string_keys;
    u32 num_entries;
    u32 capacity;
    HashmapEntry* bucket;
    u32 (*hash_func)(const u8* data, u32 len);
} GDF_HashMap_T;

static FORCEINLINE u32 __get_idx(
    void* key, 
    GDF_HashMap map
)
{
    u16 idx = map->hash_func((const u8*)key, map->k_stride) % map->capacity;
    return idx;
}

static FORCEINLINE u32 __get_idx_custom(
    void* key, 
    u32 key_size,
    u32 capacity,
    u32 (*hash_func)(const u8* data, u32 len)
)
{
    u16 idx = hash_func((const u8*)key, key_size) % capacity;
    return idx;
}

// Inserts key and value at the given index or linearly probes for an
// empty slot.
// Returns the slot modified.
// TODO! this should return NULL if anything went wrong.
static FORCEINLINE HashmapEntry* __insert(
    u32 start_idx, 
    void* key,
    u32 key_size,
    void* val,
    u32 val_size,
    u32 capacity,
    HashmapEntry* bucket
)
{
    u32 idx;

    for (u32 i = 0; i < capacity; i++)
    {
        idx = (start_idx + i) % capacity;

        if (bucket[idx].key != NULL && memcmp(bucket[idx].key, key, key_size) == 0)
        {
            // update entry
            GDF_MemCopy(bucket[idx].val, val, val_size);
            return bucket + idx;  
        }

        if (bucket[idx].key == NULL)
        {
            bucket[idx].key = GDF_Malloc(key_size, GDF_MEMTAG_APPLICATION);
            GDF_MemCopy(bucket[idx].key, key, key_size);
            bucket[idx].val = GDF_Malloc(val_size, GDF_MEMTAG_APPLICATION);
            GDF_MemCopy(bucket[idx].val, val, val_size);
            return bucket + idx;  
        }
    }

    // if capacity is exceeded for some reason
    return NULL;
}

static HashmapEntry* __iter_first(HashmapEntry* bucket, u32 capacity) 
{
    for (int i = 0; i < capacity; i++)
    {
        if (bucket[i].key != NULL)
            return bucket + i;
    }
    
    return NULL;
}

static bool __iter_next(HashmapEntry* bucket, HashmapEntry** iter, u32 capacity) 
{
    u32 idx = ((*iter) - bucket) + 1;
    for (; idx < capacity; idx++)
    {
        if (bucket[idx].key != NULL)
        {
            *iter = bucket + idx;
            return true;
        }
    }

    *iter = NULL;
    return false;
}

static FORCEINLINE void __free_mapentry(HashmapEntry* map_entry)
{
    GDF_Free(map_entry->key);
    map_entry->key = NULL;
    GDF_Free(map_entry->val);
    map_entry->val = NULL;
}

GDF_HashMap __hashmap_create(u32 k_stride, u32 v_stride, u32 (*hash_func)(const u8* data, u32 len), bool string_keys)
{
    GDF_HashMap map = GDF_Malloc(sizeof(GDF_HashMap_T), GDF_MEMTAG_APPLICATION);
    map->k_stride = k_stride;
    map->v_stride = v_stride;
    map->string_keys = string_keys;
    map->num_entries = 0;
    map->capacity = 32;

    if (hash_func == NULL)
    {
        hash_func = superfasthash_wrapper;
    }

    map->hash_func = hash_func;

    map->bucket = GDF_Malloc(sizeof(HashmapEntry) * map->capacity, GDF_MEMTAG_APPLICATION);

    return map;
}

// TODO!
bool GDF_HashmapDestroy(GDF_HashMap hashmap)
{
    return false;    
}

void* GDF_HashmapInsert(GDF_HashMap hashmap, void* key, void* value)
{
    if (key == NULL)
        return NULL;
    // TODO! when num entries is almost (0.75 or 0.5)x at capacity, alloc new array
    // and rehash all entries.

    HashmapEntry* bucket = hashmap->bucket;

    // Check if map needs to be expanded
    if (hashmap->num_entries + 1 >= (hashmap->capacity / 2))
    {
        u32 new_capacity = hashmap->capacity * 2;
        HashmapEntry* new_bucket = GDF_Malloc(sizeof(HashmapEntry) * new_capacity, GDF_MEMTAG_COLLECTION);

        // Rehash all entries
        // TODO! TESTING NEEDED.
        for (u32 i = 0; i < hashmap->capacity; i++) 
        {
            if (bucket[i].key == NULL) 
                continue;

            u32 start_idx = __get_idx_custom(
                bucket[i].key, 
                hashmap->k_stride,
                new_capacity,
                hashmap->hash_func
            );
            
            // LOG_DEBUG(
            //     "REHASHING: %d, %d, %d", 
            //     *((i32*)bucket[i].key), 
            //     *((i32*)((u64)bucket[i].key + sizeof(i32))),
            //     *((i32*)((u64)bucket[i].key + sizeof(i32) * 2))
            // );
            HashmapEntry* entry = __insert(
                start_idx,
                bucket[i].key,
                hashmap->k_stride,
                bucket[i].val,
                hashmap->v_stride,
                new_capacity,
                new_bucket
            );
            __free_mapentry(bucket + i);

            entry->owner = hashmap;
        }

        GDF_Free(hashmap->bucket);
        hashmap->bucket = new_bucket;
        hashmap->capacity = new_capacity;
    }

    // Find next free index to insert in
    u32 start_idx = __get_idx(key, hashmap);
    
    HashmapEntry* entry = __insert(
        start_idx,
        key,
        hashmap->k_stride,
        value,
        hashmap->v_stride,
        hashmap->capacity,
        hashmap->bucket
    );

    if (entry == NULL)
    {
        return NULL;
    }

    entry->owner = hashmap;
    hashmap->num_entries++;

    return entry->val;
}

void* GDF_HashmapGet(GDF_HashMap hashmap, void* key)
{
    if (key == NULL)
        return NULL;
    
    HashmapEntry* bucket = hashmap->bucket;
    u32 start_idx = __get_idx(key, hashmap);
    // Linear probing, wrap around until something is found or nothing is found.
    for (u32 i = 0, idx; i < hashmap->capacity && bucket[(
        (idx = (start_idx + i) % hashmap->capacity) 
    )].key != NULL; i++)
    {
        if (memcmp(bucket[idx].key, key, hashmap->k_stride) == 0)
        {
            return bucket[idx].val;
        } 
        // debugging chunk coords
        // LOG_DEBUG(
        //     "NOT MATCHING: \n%d, %d, %d\n%d, %d, %d\n", 
        //     *((i32*)bucket[idx].key), 
        //     *((i32*)((u64)bucket[idx].key + sizeof(i32))),
        //     *((i32*)((u64)bucket[idx].key + sizeof(i32) * 2)),
        //     *((i32*)key), 
        //     *((i32*)((u64)key + sizeof(i32))),
        //     *((i32*)((u64)key + sizeof(i32) * 2))
        // );
    }

    return NULL;
}

void GDF_HashmapRemove(GDF_HashMap hashmap, void* key, void* out_val_p)
{
    if (key == NULL)
    {
        out_val_p = NULL;
        return;
    }

    HashmapEntry* bucket = hashmap->bucket;
    u32 start_idx = __get_idx(key, hashmap);

    for (u32 i = 0, idx; i < hashmap->capacity && bucket[(
        (idx = (start_idx + i) % hashmap->capacity) 
    )].key != NULL; i++)
    {
        if (memcmp(bucket[idx].key, key, hashmap->k_stride) == 0)
        {
            if (out_val_p != NULL)
            {
                GDF_MemCopy(out_val_p, bucket[idx].val, hashmap->v_stride);
            }
            __free_mapentry(&bucket[idx]);
            hashmap->num_entries--;
            return;
        } 
    }
    out_val_p = NULL;
    return;
}

u32 GDF_HashmapLen(GDF_HashMap hashmap)
{
    return hashmap->num_entries;
}

HashmapEntry* GDF_HashmapIter(GDF_HashMap hashmap)
{
    return __iter_first(hashmap->bucket, hashmap->capacity);
}

bool GDF_HashmapIterAdvance(HashmapEntry** iter)
{
    return __iter_next((*iter)->owner->bucket, iter, (*iter)->owner->capacity);
}