#pragma once

#include "core.h"
#include "core/event.h"

// TODO! UNUSED FOR NOW..
typedef enum GDF_EVENT {
    /*
    A chunk was modified
    Usage:
    Chunk* chunk = (Chunk*)sender;
    */
    GDF_EVENT_CHUNK_UPDATE = 0x100,
    /*
    Whenever a chunk is loaded or created.
    Usage:
    Chunk* chunk = (Chunk*)sender;
    */
    GDF_EVENT_CHUNK_LOAD = 0x101,
    /*
    Fires if an Entity touches a block.
    Usage:
    Block* block = (Block*)sender
    Entity* h = (Entity*)(ctx.data.u64[0])
    */
    GDF_EVENT_BLOCK_TOUCHED = 0x102,
    
    // Not actaully an event.
    GDF_EVENT_MAX = 0xFFF

} GDF_EVENT; 