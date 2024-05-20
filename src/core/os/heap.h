#pragma once
#include "core.h"

// perhaps later change to multiple heap model
// save like 0.2 fps idk

// bytes
const u32 __HEAP_GROW_RATE = MB_TO_B(16);

bool __init_heap();
// TODO! this may cause a memory leak idk how virtualfree works yet
void __destroy_heap();

// returns NULL on failure, heap is probably full
void* __heap_alloc(u64 size, GDF_MEMTAG tag, bool aligned);
// returns false on failure, wtf are you doing
bool __heap_expand();
// returns the memory tag of the block freed
GDF_MEMTAG __heap_free(void* block, u32* size_freed, bool aligned);
void __heap_zero(void* block);
// returns false if the dest block size is less than src block size
void __heap_copy(void* dest, void* src);
void __heap_set(void* dest, i32 val);