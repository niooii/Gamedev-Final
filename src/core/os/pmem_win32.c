#include "pmem.h"

#ifdef OS_WINDOWS
#include <stdlib.h>
#include <string.h> 

void* pAlloc(u64 size, bool alligned)
{
    return malloc(size);
}

void pFree(void* block, bool alligned)
{
    return free(block);
}

void* pZero(void* block, u64 size)
{
    return memset(block, 0, size);
}

void* pCpy(void* dest, void* src, u64 size)
{
    return memcpy(dest, src, size);
}

void* pSet(void* dest, i32 val, u64 size)
{
    return memset(dest, val, size);
}

#endif