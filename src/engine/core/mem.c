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
    "LIST         ",
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
    "IO           ",
    "FREE         "
};

static struct memory_stats stats;

bool GDF_InitMemory() 
{
    GDF_MemZero(&stats, sizeof(stats));

    return true;
}

void GDF_ShutdownMemory() 
{

}

// TODO! make memtag do something or remove it
void* GDF_Malloc(u64 size, GDF_MEMTAG tag) 
{
    if (tag == GDF_MEMTAG_UNKNOWN) {
        LOG_WARN("GDF_Malloc called using GDF_MEMTAG_UNKNOWN. Re-class this allocation.");
    }

    // u32 total_allocated = 0;
    void* block = malloc(size);
    if (block == NULL)
    {
        LOG_FATAL("It appears you have ran out of memory. womp womp");
    }
    
    memzero(block, size);
    // stats.total_allocated += total_allocated;
    // stats.tagged_allocations[tag] += total_allocated;
    return block;
}

void GDF_Free(void* block) 
{
    free(block);
    // if (tag == GDF_MEMTAG_FREE)
    // {
    //     LOG_WARN("Calling GDF_Free on already free memory.");
    //     return;
    // }
    // if (tag == GDF_MEMTAG_UNKNOWN) {
    //     LOG_WARN("GDF_Free called using GDF_MEMTAG_UNKNOWN. Re-class this allocation.");
    // }
    // stats.total_allocated -= size;
    // stats.tagged_allocations[tag] -= size;
}

void GDF_MemZero(void* block, u64 size)
{
    memzero(block, size);
}

// TODO! copy over memtag and shi
void GDF_MemCopy(void* dest, void* src, u64 size)
{
    memcpy(dest, src, size);
}

// TODO!
void GDF_MemSet(void* block, i32 val, u64 size)
{
    memset(block, val, size);
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
    strcpy(out_str, buffer);
}