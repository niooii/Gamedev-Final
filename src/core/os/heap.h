#pragma once

#include "def.h"
#include "core/logging.h"
#include "core/asserts.h"

#define __HEAP_GROW_RATE MB_TO_B(16)
#define ALIGNMENT 32 
#define MIN_BLOCK_SIZE 32 

// perhaps later change to multiple heap model
// save like 0.2 fps idk

// when changing any update the corresponding string in mem.c
typedef enum GDF_MEMTAG {
    // For temporary use. Should be assigned one of the below or have a new tag created.
    GDF_MEMTAG_UNKNOWN,
    GDF_MEMTAG_ARRAY,
    GDF_MEMTAG_LIST,
    GDF_MEMTAG_DICT,
    GDF_MEMTAG_RING_QUEUE,
    GDF_MEMTAG_BST,
    GDF_MEMTAG_STRING,
    GDF_MEMTAG_APPLICATION,
    GDF_MEMTAG_JOB,
    GDF_MEMTAG_TEXTURE,
    GDF_MEMTAG_MATERIAL_INSTANCE,
    GDF_MEMTAG_RENDERER,
    GDF_MEMTAG_GAME,
    GDF_MEMTAG_TRANSFORM,
    GDF_MEMTAG_ENTITY,
    GDF_MEMTAG_ENTITY_NODE,
    GDF_MEMTAG_SCENE,
    GDF_MEMTAG_TEMP_RESOURCE,
    GDF_MEMTAG_FREE,

    GDF_MEMTAG_MAX_TAGS
} GDF_MEMTAG;

// Each heapchunk metadata struct is stored exactly sizeof(heap_chunk) bytes 
// before the address returned by GDF_Malloc().
struct heap_chunk {
    u64 size;
    GDF_MEMTAG tag;
    struct heap_chunk* next;
};

struct heap {
    struct heap_chunk* free_lists[10]; // Segregated free lists for different size ranges
    u64 available_mem;
};

bool __init_heap();
// TODO! this may cause a memory leak idk how virtualfree works yet
void __destroy_heap();

// returns NULL on failure, heap is probably full
void* __heap_alloc(u64 size, u64* total_allocated, GDF_MEMTAG tag, bool aligned);
// returns false on failure, wtf are you doing
bool __heap_expand();
bool __heap_shrink();
// returns the memory tag of the block freed
GDF_MEMTAG __heap_free(void* block, u64* size_freed, bool aligned);
void __heap_zero(void* block);
// returns false if the dest block size is less than src block size
bool __heap_copy(void* dest, void* src);
bool __heap_copy_sized(void* dest, void* src, u64 size);
void __heap_set(void* dest, i32 val);