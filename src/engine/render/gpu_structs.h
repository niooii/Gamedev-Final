#pragma once

#include "core.h"
#include "game/world/chunk.h"

typedef struct GpuChunkData {
    u64 faces[MAX_CHUNK_XZ * MAX_CHUNK_XZ * MAX_CHUNK_Y];
    u8 chunk_x;
    u8 chunk_y;
    u8 chunk_z;
} GpuChunkData;