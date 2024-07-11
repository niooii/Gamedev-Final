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
    u8 mbutton_states_current[GDF_MBUTTON_MAX];
    u8 mbutton_states_previous[GDF_MBUTTON_MAX];
} input_state;

static bool initialized = false;
static input_state state;
static GDF_CursorLockState cursor_lock_state = GDF_CursorLockState_Free;
static i16 locked_x;
static i16 locked_y;

void GDF_InitInput() 
{
    GDF_MemZero(&state, sizeof(input_state));
    initialized = true;
    LOG_INFO("Input subsystem initialized.");
}

void GDF_ShutdownInput() 
{
    // TODO: shutdown routine later
    initialized = false;
}

void GDF_INPUT_Update(f64 delta_time) 
{
    if (!initialized)
        return;

    // Copy current states to previous states.
    GDF_MemCopy(&state.keyboard_previous, &state.keyboard_current, sizeof(keyboard_state));
    GDF_MemCopy(&state.mbutton_states_previous, &state.mbutton_states_current, sizeof(state.mbutton_states_current));

    if (cursor_lock_state == GDF_CursorLockState_Locked) 
    {
        // TODO! rename info.h to misc.h and slap a (CROSS PLATFORM) GDF_SetCursorPos in there and call that instead.
        i16 screen_offset_x, screen_offset_y, w, h;
        GDF_GetWindowPos(&screen_offset_x, &screen_offset_y);
        GDF_GetWindowSize(&w, &h);
        // TODO! these assignments get repetitive
        locked_x = w/2;
        locked_y = h/2;
        SetCursorPos(locked_x + screen_offset_x, locked_y + screen_offset_y);
        state.mpos_previous.x = locked_x;
        state.mpos_previous.y = locked_y;
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

void GDF_INPUT_SetMouseLockState(GDF_CursorLockState lock_state) 
{
    cursor_lock_state = lock_state;
}

void GDF_INPUT_GetMouseDelta(i32* dx, i32* dy) 
{
    LOG_DEBUG("curr: %d, %d | %d, %d: prev", state.mpos_current.x, state.mpos_current.y, state.mpos_previous.x, state.mpos_previous.y);
    *dx = state.mpos_current.x - state.mpos_previous.x;
    *dy = state.mpos_current.y - state.mpos_previous.y;
}

void __input_process_key(GDF_KEYCODE key, bool pressed) 
{
    // check if state changed
    if (state.keyboard_current.key_states[key] != pressed) 
    {
        state.keyboard_current.key_states[key] = pressed;

        GDF_EventCtx context;
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
        GDF_EventCtx context;
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

        GDF_EventCtx context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;
        GDF_EVENT_Fire(GDF_EVENT_INTERNAL_MOUSE_MOVED, NULL, context);
    }
}

void __input_process_mouse_wheel(i8 z_delta) 
{
    GDF_EventCtx context;
    context.data.u8[0] = z_delta;
    GDF_EVENT_Fire(GDF_EVENT_INTERNAL_MOUSE_WHEEL, NULL, context);
}