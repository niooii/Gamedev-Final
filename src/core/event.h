#pragma once

#include "core.h"

typedef struct GDF_EventContext {
    // 196 bytes
    union {
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];

        i32 i32[4];
        u32 u32[4];
        f32 f32[4];

        i16 i16[8];
        u16 u16[8];

        i16 i8[16];
        u16 u8[16];

        bool b;

        char c[16];
    } data;
} GDF_EventContext;

typedef bool (*GDF_EventHandlerFP)(u16 event_code, void* sender, void* listener_instance, GDF_EventContext ctx);

bool GDF_InitEvents();
void GDF_ShutdownEvents();

bool GDF_EVENT_Register(u16 event_code, void* listener, GDF_EventHandlerFP callback);
bool GDF_EVENT_Unregister(u16 event_code, void* listener, GDF_EventHandlerFP callback);
bool GDF_EVENT_Fire(u16 event_code, void* sender, GDF_EventContext ctx);

typedef enum GDF_EVENT_INTERNAL {
    
    GDF_EVENT_INTERNAL_APP_QUIT = 0x01,
    /*
    Usage:
    u16 keycode = ctx.data.u16[0]
    */
    GDF_EVENT_INTERNAL_KEY_PRESSED = 0x02,
    /*
    Usage:
    u16 keycode = ctx.data.u16[0]
    */
    GDF_EVENT_INTERNAL_KEY_RELEASED = 0x03,
    /*
    Usage:
    u16 button = ctx.data.u16[0]
    */
    GDF_EVENT_INTERNAL_MBUTTON_PRESSED = 0x04,
    /*
    Usage:
    u16 button = ctx.data.u16[0]
    */
    GDF_EVENT_INTERNAL_MBUTTON_RELEASED = 0x05,
    /*
    Usage:
    u16 x = ctx.data.u16[0]
    u16 y = ctx.data.u16[1]
    */
    GDF_EVENT_INTERNAL_MOUSE_MOVED = 0x06,
    /*
    Usage:
    u16 dy = ctx.data.u8[0]
    */
    GDF_EVENT_INTERNAL_MOUSE_WHEEL = 0x07,
    /*
    Usage:
    u16 dx = ctx.data.i16[0]
    u16 dy = ctx.data.i16[1]
    */
    GDF_EVENT_INTERNAL_MOUSE_RAW_MOVE = 0x08,
    /*
    Usage:
    u16 w = ctx.data.u16[0]
    u16 h = ctx.data.u16[1]
    */
    GDF_EVENT_INTERNAL_WINDOW_RESIZE = 0x09,
    /*
    Usage:
    bool focus_gained = ctx.data.b;
    bool focus_lost = !ctx.data.b;
    */
    GDF_EVENT_INTERNAL_WINDOW_FOCUS_CHANGE = 0x0A,

    // Not actaully an event.
    GDF_EVENT_INTERNAL_MAX = 0xFF

} GDF_EVENT_INTERNAL; 