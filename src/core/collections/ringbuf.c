#include "ringbuf.h"

typedef struct GDF_RingBuf_T {
    u32 next_read_idx;
    u32 next_write_idx;
    u32 capacity;
    u32 stride;
} GDF_RingBuf_T;

GDF_RingBuf __create_ringbuf(u32 stride, u32 capacity)
{
    GDF_RingBuf buf = GDF_Malloc(sizeof(GDF_RingBuf_T), GDF_MEMTAG_APPLICATION);
    buf->stride = stride;
    // buf->next_idx = 0;
    buf->capacity = capacity;
    return buf;
}

void GDF_RingBufAdd(GDF_RingBuf buf, void* val)
{

}