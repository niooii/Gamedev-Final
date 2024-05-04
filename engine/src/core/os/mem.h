#pragma once
#include "def.h"

void* GDF_Alloc(u64 size, bool alligned);
void GDF_Free(void* block, bool alligned);
void* GDF_ZeroMem(void* block, u64 size);
void* GDF_MemCopy(void* dest, void* src, u64 size);
void* GDF_MemSet(void* dest, i32 val, u64 size);