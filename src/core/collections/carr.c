#include "carr.h"

typedef struct GDF_CArray_T {
    u32 next_read_idx;
    u32 next_write_idx;
    u32 capacity;
    u32 stride;
    void* data;
    bool* ready;
} GDF_CArray_T;

GDF_CArray __create_carray(u32 stride, u32 capacity)
{
    GDF_CArray buf = GDF_Malloc(sizeof(GDF_CArray_T), GDF_MEMTAG_APPLICATION);
    buf->next_read_idx = 0;
    buf->next_write_idx = 0;
    buf->capacity = capacity;
    buf->stride = stride;
    buf->ready = GDF_Malloc(sizeof(bool) * capacity, GDF_MEMTAG_APPLICATION);
    buf->data = GDF_Malloc(stride * capacity, GDF_MEMTAG_APPLICATION);
    
    return buf;
}

void* GDF_CArrayWriteNext(GDF_CArray arr)
{
    arr->ready[arr->next_write_idx] = true;
    void* data = &arr->data[arr->next_write_idx * arr->stride];
    // Wrap around index
    arr->next_write_idx = (arr->next_write_idx + 1) % arr->capacity;
    return data;
}

const void* const GDF_CArrayReadNext(GDF_CArray arr)
{
    bool* ready = &arr->ready[arr->next_read_idx];
    if (!*ready)
        return NULL;
    *ready = false;
    const void* const data = &arr->data[arr->next_read_idx * arr->stride];
    // Wrap around index
    arr->next_read_idx = (arr->next_read_idx + 1) % arr->capacity;
    return data;
}

void* GDF_CArrayGetData(GDF_CArray arr)
{
    return arr->data;
}