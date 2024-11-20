#pragma once

#include "core.h"
#include "game/world/chunk.h"

typedef struct GpuChunkData {
    u64 faces[CHUNK_SIZE_XZ * CHUNK_SIZE_XZ * CHUNK_SIZE_Y];
    u8 chunk_x;
    u8 chunk_y;
    u8 chunk_z;
} GpuChunkData;