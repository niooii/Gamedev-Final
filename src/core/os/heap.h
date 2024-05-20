#pragma once
#include "core.h"

// perhaps later change to multiple heap model
// save like 0.2 fps idk

// bytes
const u32 __HEAP_GROW_RATE = MB_TO_B(128);

void __init_heap();
void __destroy_heap();

void* __heap_alloc(u64 size, bool alligned);
void __heap_free(void* block, bool alligned);
// TODO! no size parameter this will set all bytes to 0
void* __heap_zero(void* block);
void* __heap_copy(void* dest, void* src, u64 size);
// this will keep size in case we want specific operations
void* __heap_set(void* dest, i32 val, u64 size);