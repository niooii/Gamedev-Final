#pragma once

#include "core.h"
#include "core/event.h"

// TODO! UNUSED FOR NOW..
typedef enum GDF_EVENT {
    /*
    Usage:
    u16 keycode = ctx.data.u16[0]
    */
    GDF_EVENT_CHUNK_CHANGE = 0x100,
    /*
    Usage:
    u16 keycode = ctx.data.u16[0]
    */
    GDF_EVENT_INTERNAL_ENTITY_HIT = 0x101,
    
    // Not actaully an event.
    GDF_EVENT_MAX = 0xFFF

} GDF_EVENT; 