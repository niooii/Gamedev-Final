#include "mem.h"

#include "core.h"

// TODO: Custom string lib
#include <string.h>
#include <stdio.h>

struct memory_stats {
    u64 total_allocated;
    u64 tagged_allocations[GDF_MEMTAG_MAX_TAGS];
};

static const char* GDF_MEMTAG_strings[GDF_MEMTAG_MAX_TAGS] = {
    "UNKNOWN      ",
    "ARRAY        ",
    "DARRAY       ",
    "DICT         ",
    "RING_QUEUE   ",
    "BST          ",
    "STRING       ",
    "APPLICATION  ",
    "JOB          ",
    "TEXTURE      ",
    "MAT_INST     ",
    "RENDERER     ",
    "GAME         ",
    "TRANSFORM    ",
    "ENTITY       ",
    "ENTITY_NODE  ",
    "SCENE        ",
    "TEMP RESOURCE",
    "FREE         "
};

static struct memory_stats stats;

bool GDF_InitMemory() 
{
    GDF_MemZero(&stats, sizeof(stats));
    if (!__init_heap())
    {
        LOG_FATAL("Failed to initialize the heap.");
        return false;
    }

    return true;
}

void GDF_ShutdownMemory() 
{

}

void* GDF_Malloc(u64 size, GDF_MEMTAG tag) 
{
    if (tag == GDF_MEMTAG_UNKNOWN) {
        LOG_WARN("GDF_Malloc called using GDF_MEMTAG_UNKNOWN. Re-class this allocation.");
    }

    // TODO: Memory alignment
    void* block = __heap_alloc(size, tag, false);
    if (block == NULL)
    {
        LOG_ERR("Could not allocate block (%d bytes), expanding heap...", size);
        if (!__heap_expand())
        {
            LOG_FATAL("Could not expand heap, wtf are you doing man.");
            return NULL;
        }
        block = __heap_alloc(size, tag, false);
        if (block == NULL)
        {
            LOG_FATAL("Just expanded heap but heap_alloc returned NULL, might wanna get that checked out.");
            return NULL;
        }
    }
    __heap_zero(block);
    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;
    return block;
}

void GDF_Free(void* block) 
{
    // TODO: Memory alignment
    u32 size_freed = 0;
    GDF_MEMTAG tag = __heap_free(block, &size_freed, false);

    if (tag == GDF_MEMTAG_UNKNOWN) {
        LOG_WARN("GDF_Free called using GDF_MEMTAG_UNKNOWN. Re-class this allocation.");
    }

    stats.total_allocated -= size_freed;
    stats.tagged_allocations[tag] -= size_freed;
}

void GDF_HeapZero(void* block) 
{
    __heap_zero(block);
}

void GDF_HeapCopy(void* dest, const void* source) 
{
    __heap_copy(dest, source);
}

void GDF_HeapSet(void* dest, i32 value) 
{
    __heap_set(dest, value);
}

void GDF_MemZero(void* block, u64 size)
{

}

void GDF_MemCopy(void* block, u64 size)
{
    
}

void GDF_MemSet(void* block, u64 size)
{

}

void GDF_GetMemUsageStr(char* out_str) 
{
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    char buffer[8000] = "System memory use (tagged):\n";
    u64 offset = strlen(buffer);
    for (u32 i = 0; i < GDF_MEMTAG_MAX_TAGS; ++i) {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if (stats.tagged_allocations[i] >= gib) {
            unit[0] = 'G';
            amount = stats.tagged_allocations[i] / (float)gib;
        } else if (stats.tagged_allocations[i] >= mib) {
            unit[0] = 'M';
            amount = stats.tagged_allocations[i] / (float)mib;
        } else if (stats.tagged_allocations[i] >= kib) {
            unit[0] = 'K';
            amount = stats.tagged_allocations[i] / (float)kib;
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (float)stats.tagged_allocations[i];
        }

        i32 length = snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", GDF_MEMTAG_strings[i], amount, unit);
        offset += length;
    }
    out_str = strdup(buffer);
}