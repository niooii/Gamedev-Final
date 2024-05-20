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
    "UNKNOWN    ",
    "ARRAY      ",
    "DARRAY     ",
    "DICT       ",
    "RING_QUEUE ",
    "BST        ",
    "STRING     ",
    "APPLICATION",
    "JOB        ",
    "TEXTURE    ",
    "MAT_INST   ",
    "RENDERER   ",
    "GAME       ",
    "TRANSFORM  ",
    "ENTITY     ",
    "ENTITY_NODE",
    "SCENE      "};

static struct memory_stats stats;

void GDF_InitMemory() {
    pZero(&stats, sizeof(stats));
}

void GDF_ShutdownMemory() {

}

void* GDF_Malloc(u64 size, GDF_MemTag tag) {
    if (tag == GDF_MEMTAG_UNKNOWN) {
        LOG_WARN("GDF_Malloc called using GDF_MEMTAG_UNKNOWN. Re-class this allocation.");
    }

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;

    // TODO: Memory alignment
    void* block = pAlloc(size, false);
    pZero(block, size);
    return block;
}

void GDF_Free(void* block, u64 size, GDF_MemTag tag) {
    if (tag == GDF_MEMTAG_UNKNOWN) {
        LOG_WARN("GDF_Free called using GDF_MEMTAG_UNKNOWN. Re-class this allocation.");
    }

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

    // TODO: Memory alignment
    pFree(block, false);
}

void* GDF_MZero(void* block, u64 size) {
    return pZero(block, size);
}

void* GDF_Memcpy(void* dest, const void* source, u64 size) {
    return pCpy(dest, source, size);
}

void* GDF_Memset(void* dest, i32 value, u64 size) {
    return pSet(dest, value, size);
}

char* GDF_GetMemUsageStr() {
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
    char* out_string = _strdup(buffer);
    return out_string;
}