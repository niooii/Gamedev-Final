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

// If the address passed in is not at the start of a block (the address returned by GDF_Malloc),
// expect undefined behavior. 
void GDF_Free(void* block_start);

// If the address passed in is not at the start of a block (the address returned by GDF_Malloc),
// expect undefined behavior. 
void GDF_HeapZero(void* block_start);

// If the addresses passed in are not at the start of a block (the address returned by GDF_Malloc),
// expect undefined behavior. 
void GDF_HeapCopy(void* dest_start, const void* source_start);

// Addresses passed in can be anywhere in allocated memory. 
void GDF_HeapCopyS(void* dest, const void* source, u64 size);

// If the address passed in is not at the start of a block (the address returned by GDF_Malloc),
// expect undefined behavior. 
void GDF_HeapSet(void* dest_start, i32 value);

inline void GDF_MemZero(void* block, u64 size)
{
    memset(block, 0, size);
}

// TODO!
void GDF_MemCopy(void* dest, void* src, u64 size);

// TODO!
void GDF_MemSet(void* block, u64 size);

void GDF_GetMemUsageStr(char* out_str);