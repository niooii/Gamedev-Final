#pragma once
#include "core.h"

// platform memory allocate
void* pAlloc(u64 size, bool alligned);
// platform memory free
void pFree(void* block, bool alligned);
// platform memset to zero
void* pZero(void* block, u64 size);
// platform memcpy
void* pCpy(void* dest, void* src, u64 size);
// platform set
void* pSet(void* dest, i32 val, u64 size);