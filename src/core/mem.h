#pragma once

#include "core.h"

typedef enum GDF_MemoryTag {
    // For temporary use. Should be assigned one of the below or have a new tag created.
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_DARRAY,
    MEMORY_TAG_DICT,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BST,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    MEMORY_TAG_MAX_TAGS
} GDF_MemoryTag;

void initialize_memory();
void shutdown_memory();

void* GDF_Malloc(u64 size, GDF_MemoryTag tag);

void GDF_Free(void* block, u64 size, GDF_MemoryTag tag);

void* GDF_MZero(void* block, u64 size);

void* GDF_Memcpy(void* dest, const void* source, u64 size);

void* GDF_Memset(void* dest, i32 value, u64 size);

char* GDF_GetMemUsageStr();