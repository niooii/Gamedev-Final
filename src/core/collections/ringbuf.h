#pragma once

#include "core.h"

typedef struct GDF_RingBuf_T* GDF_RingBuf;

GDF_RingBuf __create_ringbuf(u32 stride, u32 capacity);
#define GDF_CreateRingbuf(type, capacity) \
    __create_ringbuf(sizeof(type), capacity);

void GDF_RingBufPush(GDF_RingBuf buf, void* val);
void* GDF_RingBufPop(GDF_RingBuf buf);