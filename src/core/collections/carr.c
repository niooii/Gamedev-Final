#include "carr.h"

typedef struct GDF_CArray_T {
    u32 next_read_idx;
    u32 next_write_idx;
    u32 capacity;
    u32 stride;
    void* data;
} GDF_CArray_T;

GDF_CArray __create_carray(u32 stride, u32 capacity)
{
    GDF_CArray buf = GDF_Malloc(sizeof(GDF_CArray_T), GDF_MEMTAG_APPLICATION);
    buf->next_read_idx = 0;
    buf->next_write_idx = 0;
    buf->capacity = capacity;
    buf->stride = stride;
    buf->data = GDF_Malloc(stride * capacity, GDF_MEMTAG_APPLICATION);
    return buf;
}

void* GDF_CArrayWriteNext(GDF_CArray arr)
{
    // TODO!
}
const void const* GDF_CArrayReadNext(GDF_CArray arr)
{
    // TODO!
}