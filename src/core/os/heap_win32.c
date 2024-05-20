#include "heap.h"

#ifdef OS_WINDOWS
#include <stdlib.h>
#include <string.h> 

struct heap {
    struct heap_chunk* next;
    u32 available_mem;
};

struct heap_chunk {
    u32 size;
    bool in_use;
    struct heap_chunk* next;
};

static struct heap heap;

// https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc
void __init_heap() 
{

}

void __destroy_heap()
{
    
}

void* __heap_alloc(u64 size, bool alligned)
{

}

void __heap_free(void* block, bool alligned)
{

}

// TODO! no size parameter this will set all bytes to 0
void* __heap_zero(void* block)
{

}

void* __heap_copy(void* dest, void* src, u64 size);
{

}

// this will keep size in case we want specific operations
void* __heap_set(void* dest, i32 val, u64 size)
{

}

#endif