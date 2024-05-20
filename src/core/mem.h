#pragma once

#include "core.h"
#include "core/os/heap.h"
#ifdef OS_WINDOWS
#include <windows.h>
#define strdup(p) _strdup(p)
#endif

typedef enum GDF_MemTag {
    // For temporary use. Should be assigned one of the below or have a new tag created.
    GDF_MEMTAG_UNKNOWN,
    GDF_MEMTAG_ARRAY,
    GDF_MEMTAG_DARRAY,
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

    GDF_MEMTAG_MAX_TAGS
} GDF_MemTag;

void GDF_InitMemory();
void GDF_ShutdownMemory();

void* GDF_Malloc(u64 size, GDF_MemTag tag);

void GDF_Free(void* block, u64 size, GDF_MemTag tag);

void* GDF_MZero(void* block, u64 size);

void* GDF_Memcpy(void* dest, const void* source, u64 size);

void* GDF_Memset(void* dest, i32 value, u64 size);

char* GDF_GetMemUsageStr();