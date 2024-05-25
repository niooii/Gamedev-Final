#include "heap.h"

#ifdef OS_WINDOWS
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <stdint.h>
#include <Windows.h>

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

static struct heap heap;

bool __init_heap() 
{
    // Allocate memory for the heap
    struct heap_chunk* initial_chunk = (struct heap_chunk*)VirtualAlloc(NULL, __HEAP_GROW_RATE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (initial_chunk == NULL)
    {
        return false;
    }

    heap.available_mem = __HEAP_GROW_RATE - sizeof(struct heap_chunk);

    initial_chunk->size = heap.available_mem;
    initial_chunk->tag = GDF_MEMTAG_FREE;
    initial_chunk->next = NULL;

    memset(heap.free_lists, 0, sizeof(heap.free_lists));
    heap.free_lists[9] = initial_chunk; // Insert the initial chunk into the largest free list

    return true;
}

void __destroy_heap()
{
    // Free the allocated memory for the heap
    VirtualFree(heap.free_lists[9], 0, MEM_RELEASE);
}

static int get_free_list_index(u64 size)
{
    // Determine the index for the free list based on block size
    int index = 0;
    size >>= 5;
    while (size > 1) {
        size >>= 1;
        index++;
    }
    return (index < 9) ? index : 9;
}

static void insert_free_block(struct heap_chunk* chunk)
{
    int index = get_free_list_index(chunk->size);
    chunk->next = heap.free_lists[index];
    heap.free_lists[index] = chunk;
}

static struct heap_chunk* find_best_fit(u64 size)
{
    int index = get_free_list_index(size);
    for (int i = index; i < 10; i++) 
    {
        struct heap_chunk* current = heap.free_lists[i];
        struct heap_chunk* previous = NULL;

        while (current) 
        {
            if (current->size >= size) 
            {
                if (previous) 
                {
                    previous->next = current->next;
                } 
                else 
                {
                    heap.free_lists[i] = current->next;
                }
                return current;
            }
            previous = current;
            current = current->next;
        }
    }
    return NULL;
}

void* __heap_alloc(u64 size, u64* total_allocated, GDF_MEMTAG tag, bool aligned)
{
    if (aligned) 
    {
        size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    }
    size = (size < MIN_BLOCK_SIZE) ? MIN_BLOCK_SIZE : size;
    *total_allocated = size;
    size += sizeof(struct heap_chunk);

    struct heap_chunk* chunk = find_best_fit(size);

    if (!chunk) 
    {
        if (!__heap_expand()) 
        {
            return NULL;
        }
        chunk = find_best_fit(size);
        if (!chunk) 
        {
            return NULL;
        }
    }

    if (chunk->size >= size + sizeof(struct heap_chunk) + MIN_BLOCK_SIZE) 
    {
        struct heap_chunk* new_chunk = (struct heap_chunk*)((char*)chunk + size);
        new_chunk->size = chunk->size - size;
        new_chunk->tag = GDF_MEMTAG_FREE;
        chunk->size = size;

        insert_free_block(new_chunk);
    }

    chunk->tag = tag;
    heap.available_mem -= chunk->size;

    return (char*)chunk + sizeof(struct heap_chunk);
}

bool __heap_expand()
{
    struct heap_chunk* new_heap_chunk = (struct heap_chunk*)VirtualAlloc(NULL, __HEAP_GROW_RATE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!new_heap_chunk) {
        return false;
    }

    new_heap_chunk->size = __HEAP_GROW_RATE - sizeof(struct heap_chunk);
    new_heap_chunk->tag = GDF_MEMTAG_FREE;
    new_heap_chunk->next = NULL;

    insert_free_block(new_heap_chunk);
    heap.available_mem += new_heap_chunk->size;

    return true;
}

GDF_MEMTAG __heap_free(void* block, u64* size_freed, bool aligned)
{
    struct heap_chunk* chunk = (struct heap_chunk*)((char*)block - sizeof(struct heap_chunk));
    GDF_MEMTAG old_tag = chunk->tag;
    chunk->tag = GDF_MEMTAG_FREE;
    *size_freed = chunk->size - sizeof(struct heap_chunk);

    heap.available_mem += chunk->size;

    insert_free_block(chunk);

    return old_tag;
}

void __heap_zero(void* block)
{
    struct heap_chunk* chunk = (struct heap_chunk*)((char*)block - sizeof(struct heap_chunk));
    memset(block, 0, chunk->size - sizeof(struct heap_chunk));
}

bool __heap_copy(void* dest, void* src)
{
    struct heap_chunk* src_chunk = (struct heap_chunk*)((char*)src - sizeof(struct heap_chunk));
    return __heap_copy_sized(dest, src, src_chunk->size - sizeof(struct heap_chunk));
}

bool __heap_copy_sized(void* dest, void* src, u64 size)
{
    struct heap_chunk* dest_chunk = (struct heap_chunk*)((char*)dest - sizeof(struct heap_chunk));
    struct heap_chunk* src_chunk = (struct heap_chunk*)((char*)src - sizeof(struct heap_chunk));

    if (dest_chunk->size - sizeof(struct heap_chunk) < size) {
        LOG_WARN("Requested size of data to copy is larger than the size of the dest block (%lu > %lu). Abandoning operation.", size, src_chunk->size - sizeof(struct heap_chunk));
        return false;
    }

    if (src_chunk->size - sizeof(struct heap_chunk) < size)
    {
        LOG_WARN("Requested size of data to copy is larger than the size of the src block (%lu > %lu). Abandoning operation.", size, src_chunk->size - sizeof(struct heap_chunk));
        return false;
    }

    memcpy(dest, src, size);
    return true;
}

void __heap_set(void* dest, int val)
{
    struct heap_chunk* dest_chunk = (struct heap_chunk*)((char*)dest - sizeof(struct heap_chunk));
    memset(dest, val, dest_chunk->size - sizeof(struct heap_chunk));
}

#endif
