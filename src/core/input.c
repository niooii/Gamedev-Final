#include "input.h"
#include "core/event.h"

typedef struct keyboard_state {
    bool key_states[256];
} keyboard_state;

typedef struct mouse_state {
    i16 x;
    i16 y;
    u8 mbutton_states[GDF_MBUTTON_MAX];
} mouse_state;

typedef struct input_state {
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_state mouse_current;
    mouse_state mouse_previous;
} input_state;

static bool initialized = FALSE;
static input_state state = {};

void GDF_InitInput() {
    GDF_MemZero(&state, sizeof(input_state));
    initialized = TRUE;
    LOG_INFO("Input subsystem initialized.");
}

void GDF_ShutdownInput() {
    // TODO: shutdown routine later
    initialized = FALSE;
}

void GDF_INPUT_Update(f64 delta_time) {
    if (!initialized)
        return;

    // Copy current states to previous states.
    GDF_MemCopy(&state.keyboard_previous, &state.keyboard_current, sizeof(keyboard_state));
    GDF_MemCopy(&state.mouse_previous, &state.mouse_current, sizeof(mouse_state));
}

bool GDF_INPUT_IsKeyDown(GDF_KEYCODE key) {
    if (!initialized) {
        return FALSE;
    }
    return state.keyboard_current.key_states[key] == TRUE;
}

bool GDF_INPUT_WasKeyDown(GDF_KEYCODE key) {
    if (!initialized) {
        return FALSE;
    }
    return state.keyboard_previous.key_states[key] == TRUE;
}

// mouse input
bool GDF_INPUT_IsButtonDown(GDF_MBUTTON button) {
    if (!initialized) {
        return FALSE;
    }
    return state.mouse_current.mbutton_states[button] == TRUE;
}

bool GDF_INPUT_WasButtonDown(GDF_MBUTTON button) {
    if (!initialized) {
        return FALSE;
    }
    return state.mouse_previous.mbutton_states[button] == TRUE;
}

void GDF_INPUT_GetMousePos(i32* x, i32* y) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.mouse_current.x;
    *y = state.mouse_current.y;
}

void GDF_INPUT_GetPrevMousePos(i32* x, i32* y) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_previous.x;
    *y = state.mouse_previous.y;
}

void __input_process_key(GDF_KEYCODE key, bool pressed) {
    // check if state changed
    if (state.keyboard_current.key_states[key] != pressed) {
        state.keyboard_current.key_states[key] = pressed;

        GDF_EventCtx context;
        context.data.u16[0] = key;
        GDF_EVENT_Fire(pressed ? GDF_EVENT_INTERNAL_KEY_PRESSED : GDF_EVENT_INTERNAL_KEY_RELEASED, NULL, context);
    }
}

void __input_process_button(GDF_MBUTTON button, bool pressed) {
    // check if state changed
    if (state.mouse_current.mbutton_states[button] != pressed) {
        state.mouse_current.mbutton_states[button] = pressed;

        GDF_EventCtx context;
        context.data.u16[0] = button;
        GDF_EVENT_Fire(pressed ? GDF_EVENT_INTERNAL_MBUTTON_PRESSED : GDF_EVENT_INTERNAL_MBUTTON_RELEASED, NULL, context);
    }
}

void __input_process_mouse_move(i16 x, i16 y) {
    // check if different
    if (state.mouse_current.x != x || state.mouse_current.y != y) {
        LOG_DEBUG("Mouse pos: %i, %i!", x, y);

        state.mouse_current.x = x;
        state.mouse_current.y = y;

        GDF_EventCtx context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;
        GDF_EVENT_Fire(GDF_EVENT_INTERNAL_MOUSE_MOVED, NULL, context);
    }
}

void __input_process_mouse_wheel(i8 z_delta) {
    GDF_EventCtx context;
    context.data.u8[0] = z_delta;
    GDF_EVENT_Fire(GDF_EVENT_INTERNAL_MOUSE_WHEEL, NULL, context);
}