#include "input.h"
#include "engine/core/event.h"
#include "engine/core/os/window.h"
#include "engine/math/math_types.h"
// TODO! yes yes i know this will go away later.
#include <windows.h>

typedef struct keyboard_state {
    bool key_states[256];
} keyboard_state;

typedef struct mouse_pos {
    i16 x;
    i16 y;
} mouse_pos;

typedef struct input_state {
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_pos mpos_current;
    mouse_pos mpos_previous;
    mouse_pos mouse_delta; 
    u8 mbutton_states_current[GDF_MBUTTON_MAX];
    u8 mbutton_states_previous[GDF_MBUTTON_MAX];
} input_state;

static bool initialized = false;
static input_state state;
static GDF_CURSOR_LOCK_STATE cursor_lock_state = GDF_CURSOR_LOCK_STATE_Free;
static RECT mouse_confinement_rect;

static bool __input_system_on_event(u16 event_code, void* sender, void* listener_instance, GDF_EventContext ctx)
{
    switch (event_code)
    {
        case GDF_EVENT_INTERNAL_WINDOW_FOCUS_CHANGE:
        {
            bool focus_gained = ctx.data.b;
            if (focus_gained)
            {
                if (cursor_lock_state == GDF_CURSOR_LOCK_STATE_Locked)
                {
                    ClipCursor(&mouse_confinement_rect);
                }
            }
        }
    }

    return false;
}

// Relies on the event system being initialized first.
void GDF_InitInput() 
{
    GDF_MemZero(&state, sizeof(input_state));
    initialized = true;
    LOG_INFO("Input subsystem initialized.");

    // register to some important events for the input system
    GDF_EVENT_Register(GDF_EVENT_INTERNAL_WINDOW_FOCUS_CHANGE, NULL, __input_system_on_event);
}

void GDF_ShutdownInput() 
{
    // TODO: shutdown routine later
    initialized = false;
}

static void __update_mouse_confinement_rect() 
{
    // TODO! rename info.h to misc.h and slap a (CROSS PLATFORM) GDF_ShowCursor & GDF_ClipCursor in there and call that instead.
    i16 screen_offset_x, screen_offset_y, w, h;
    GDF_GetWindowPos(&screen_offset_x, &screen_offset_y);
    GDF_GetWindowSize(&w, &h);
    // TODO! these assignments get repetitive
    mouse_confinement_rect.bottom = screen_offset_y + h/2;
    mouse_confinement_rect.top = screen_offset_y + h/2;
    mouse_confinement_rect.right = screen_offset_x + w/2;
    mouse_confinement_rect.left = screen_offset_x + w/2;
}

void GDF_INPUT_Update(f64 delta_time) 
{
    if (!initialized)
        return;

    // Copy current states to previous states.
    GDF_MemCopy(&state.keyboard_previous, &state.keyboard_current, sizeof(keyboard_state));
    GDF_MemCopy(&state.mbutton_states_previous, &state.mbutton_states_current, sizeof(state.mbutton_states_current));
    GDF_MemZero(&state.mouse_delta, sizeof(state.mouse_delta));
    if (cursor_lock_state == GDF_CURSOR_LOCK_STATE_Locked) 
    {
        __update_mouse_confinement_rect();
        return;
    }
    GDF_MemCopy(&state.mpos_previous, &state.mpos_current, sizeof(state.mpos_current));
}

bool GDF_INPUT_IsKeyDown(GDF_KEYCODE key) 
{
    if (!initialized) 
    {
        return false;
    }
    return state.keyboard_current.key_states[key] == true;
}

bool GDF_INPUT_WasKeyDown(GDF_KEYCODE key) 
{
    if (!initialized) 
    {
        return false;
    }
    return state.keyboard_previous.key_states[key] == true;
}

// mouse input
bool GDF_INPUT_IsButtonDown(GDF_MBUTTON button) 
{
    if (!initialized) 
    {
        return false;
    }
    return state.mbutton_states_current[button] == true;
}

bool GDF_INPUT_WasButtonDown(GDF_MBUTTON button) 
{
    if (!initialized) 
    {
        return false;
    }
    return state.mbutton_states_previous[button] == true;
}

void GDF_INPUT_GetMousePos(i32* x, i32* y) 
{
    if (!initialized) 
    {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.mpos_current.x;
    *y = state.mpos_current.y;
}

void GDF_INPUT_GetPrevMousePos(i32* x, i32* y) 
{
    if (!initialized) 
    {
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mpos_previous.x;
    *y = state.mpos_previous.y;
}

void GDF_INPUT_SetMouseLockState(GDF_CURSOR_LOCK_STATE lock_state) 
{
    cursor_lock_state = lock_state;

    switch (lock_state)
    {
        case GDF_CURSOR_LOCK_STATE_Locked:
        {
            __update_mouse_confinement_rect();
            ClipCursor(&mouse_confinement_rect);
            ShowCursor(false);
            break;
        }
        case GDF_CURSOR_LOCK_STATE_Free:
        {
            ClipCursor(NULL);
            ShowCursor(true);
            break;
        }
    }
}

void GDF_INPUT_GetMouseDelta(i32* dx, i32* dy) 
{
    *dx = state.mouse_delta.x;
    *dy = state.mouse_delta.y;
    
    // reset the accumulated delta... maybe
    // state.mouse_delta_x = 0;
    // state.mouse_delta_y = 0;
}

void __input_process_key(GDF_KEYCODE key, bool pressed) 
{
    // check if state changed
    if (state.keyboard_current.key_states[key] != pressed) 
    {
        state.keyboard_current.key_states[key] = pressed;

        GDF_EventContext context;
        context.data.u16[0] = key;
        GDF_EVENT_Fire(pressed ? GDF_EVENT_INTERNAL_KEY_PRESSED : GDF_EVENT_INTERNAL_KEY_RELEASED, NULL, context);
    }
}

void __input_process_button(GDF_MBUTTON button, bool pressed) 
{
    // check if state changed
    if (state.mbutton_states_current[button] != pressed) 
    {
        state.mbutton_states_current[button] = pressed;
        GDF_EventContext context;
        context.data.u16[0] = button;
        GDF_EVENT_Fire(pressed ? GDF_EVENT_INTERNAL_MBUTTON_PRESSED : GDF_EVENT_INTERNAL_MBUTTON_RELEASED, NULL, context);
    }
}

void __input_process_mouse_move(i16 x, i16 y) 
{
    // check if different
    if (state.mpos_current.x != x || state.mpos_current.y != y) 
    {
        state.mpos_current.x = x;
        state.mpos_current.y = y;

        GDF_EventContext context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;
        GDF_EVENT_Fire(GDF_EVENT_INTERNAL_MOUSE_MOVED, NULL, context);
    }
}

void __input_process_raw_mouse_move(i32 dx, i32 dy) 
{
    state.mouse_delta.x += dx;
    state.mouse_delta.y += dy;

    GDF_EventContext context;
    context.data.i16[0] = dx;
    context.data.i16[1] = dy;
    GDF_EVENT_Fire(GDF_EVENT_INTERNAL_MOUSE_RAW_MOVE, NULL, context);
}

void __input_process_mouse_wheel(i8 z_delta) 
{
    GDF_EventContext context;
    context.data.u8[0] = z_delta;
    GDF_EVENT_Fire(GDF_EVENT_INTERNAL_MOUSE_WHEEL, NULL, context);
}