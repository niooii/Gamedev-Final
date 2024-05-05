#include "mem.h"

#ifdef OS_WINDOWS
#include <stdlib.h>
#include <string.h> 

void* GDF_Alloc(u64 size, bool alligned)
{
    return malloc(size);
}

void GDF_Free(void* block, bool alligned)
{
    return free(block);
}

void* GDF_ZeroMem(void* block, u64 size)
{
    return memset(block, 0, size);
}

void* GDF_MemCopy(void* dest, void* src, u64 size)
{
    return memcpy(dest, src, size);
}

void* GDF_MemSet(void* dest, i32 val, u64 size)
{
    return memset(dest, val, size);
}

#endif