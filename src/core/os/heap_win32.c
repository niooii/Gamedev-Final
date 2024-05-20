#include "heap.h"

#ifdef OS_WINDOWS
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <stdint.h>
#include <Windows.h>

// Define your custom type for unsigned 32-bit integer
typedef uint32_t u32;

// Constants
#define ALIGNMENT 16 // Default alignment, you can adjust it

struct heap {
    struct heap_chunk* start;
    u32 available_mem;
};
 
struct heap_chunk {
    u32 size;
    GDF_MEMTAG tag;
    struct heap_chunk* next;
};

static struct heap heap;

void __init_heap() 
{
    // Allocate memory for the heap
    heap.start = (struct heap_chunk*)VirtualAlloc(NULL, __HEAP_GROW_RATE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (heap.start == NULL)
    {
        return false;
    }

    heap.available_mem = __HEAP_GROW_RATE;
    heap.start->size = __HEAP_GROW_RATE;
    heap.start->tag = GDF_MEMTAG_FREE;
    heap.start->next = NULL;
}

void __destroy_heap()
{
    // Free the allocated memory for the heap
    VirtualFree(heap.start, 0, MEM_RELEASE);
}

void* __heap_alloc(u64 size, GDF_MEMTAG tag, bool aligned)
{
    struct heap_chunk* current = heap.start;
    struct heap_chunk* previous = NULL;
    struct heap_chunk* new_chunk = NULL;
    void* allocated_block = NULL;

    // Align size if requested
    if (aligned)
    {
        size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    }

    // Find a free chunk that fits the requested size
    while (current)
    {
        if (current->tag == GDF_MEMTAG_FREE && current->size >= size)
        {
            // If the chunk is large enough, split it
            if (current->size > size + sizeof(struct heap_chunk))
            {
                new_chunk = (struct heap_chunk*)((char*)current + size + sizeof(struct heap_chunk));
                new_chunk->size = current->size - size - sizeof(struct heap_chunk);
                new_chunk->tag = GDF_MEMTAG_FREE;
                new_chunk->next = current->next;
                current->next = new_chunk;
                current->size = size;
            }
            current->tag = tag;
            allocated_block = (char*)current + sizeof(struct heap_chunk);
            break;
        }
        previous = current;
        current = current->next;
    }

    return allocated_block;
}

bool __heap_expand()
{
    // Allocate memory for the new heap chunk
    struct heap_chunk* new_heap_chunk = (struct heap_chunk*)VirtualAlloc(NULL, __HEAP_GROW_RATE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!new_heap_chunk)
    {
        return false;
    }

    // Initialize the new heap chunk
    new_heap_chunk->size = __HEAP_GROW_RATE - sizeof(struct heap_chunk);
    new_heap_chunk->tag = GDF_MEMTAG_FREE;
    new_heap_chunk->next = NULL;

    // Find the last chunk in the list
    struct heap_chunk* last_chunk = heap.start;
    while (last_chunk->next)
    {
        last_chunk = last_chunk->next;
    }

    // Link the new chunk to the end of the list
    last_chunk->next = new_heap_chunk;

    return true;
}

GDF_MEMTAG __heap_free(void* block, u32* size_freed, bool aligned)
{
    struct heap_chunk* current = (struct heap_chunk*)((char*)block - sizeof(struct heap_chunk));
    GDF_MEMTAG old_tag = current->tag;
    current->tag = GDF_MEMTAG_FREE;
    *size_freed = current->size;

    // Update available memory
    heap.available_mem += current->size;

    // Merge consecutive free chunks
    while (current->next && current->next->tag == GDF_MEMTAG_FREE)
    {
        current->size += current->next->size + sizeof(struct heap_chunk);
        current->next = current->next->next;
    }

    // Align the heap if necessary
    if (aligned)
    {
        // Find the next aligned memory block
        u64 aligned_addr = ((u64)block + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
        u64 adjustment = aligned_addr - (u64)block;

        // Adjust the available memory
        heap.available_mem += adjustment;
        current->size -= adjustment;

        // Update the block pointer
        block = (void*)aligned_addr;
    }
    return old_tag;
}

void __heap_zero(void* block)
{
    struct heap_chunk* current = (struct heap_chunk*)((char*)block - sizeof(struct heap_chunk));
    memset(block, 0, current->size);
}

bool __heap_copy(void* dest, void* src)
{
    struct heap_chunk* dest_chunk = (struct heap_chunk*)((char*)dest - sizeof(struct heap_chunk));
    struct heap_chunk* src_chunk = (struct heap_chunk*)((char*)src - sizeof(struct heap_chunk));
    
    if (dest_chunk->size < src_chunk->size)
    {
        return false;
    }

    memcpy(dest, src, src_chunk->size);

    return true;
}

void __heap_set(void* dest, int val)
{
    struct heap_chunk* dest_chunk = (struct heap_chunk*)((char*)dest - sizeof(struct heap_chunk));

    memset(dest, val, dest_chunk->size);
}

#endif
