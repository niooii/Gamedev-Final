#pragma once

#include "core.h"
#include "core/os/heap.h"
#ifdef OS_WINDOWS
#include <windows.h>
#define strdup(p) _strdup(p)
#endif

bool GDF_InitMemory();
void GDF_ShutdownMemory();

// memory allocated with this is automatically zero'd
void* GDF_Malloc(u64 size, GDF_MEMTAG tag);

void GDF_Free(void* block);

void GDF_HeapZero(void* block);

void GDF_HeapCopy(void* dest, const void* source);
void GDF_HeapCopyS(void* dest, const void* source, u64 size);

void GDF_HeapSet(void* dest, i32 value);

// TODO!
void GDF_MemZero(void* block, u64 size);

// TODO!
void GDF_MemCopy(void* block, u64 size);

// TODO!
void GDF_MemSet(void* block, u64 size);

void GDF_GetMemUsageStr(char* out_str);