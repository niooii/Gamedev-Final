#include "event.h"
#include "core/containers/list.h"

// represents a registered listener for an event
typedef struct registered_event {
    void* listener;
    GDF_EventHandlerFP callback;
} registered_event;

// data stored for each event code
typedef struct event_code_entry {
    registered_event* registered_events_list;
} event_code_entry;

#define MAX_EVENT_CODES 16384

typedef struct EVENT_SYS_STATE {
    event_code_entry entries[MAX_EVENT_CODES];
} EVENT_SYS_STATE;

static bool INITIALIZED = false;
static EVENT_SYS_STATE state;

bool GDF_InitEvents()
{
    if (INITIALIZED)
        return false;
    GDF_MemZero(&state, sizeof(state));
    INITIALIZED = true;
    return true;
}

void GDF_ShutdownEvents()
{
    for (u16 i = 0; i < MAX_EVENT_CODES; ++i)
    {
        if (state.entries[i].registered_events_list != NULL)
        {
            GDF_LIST_Destroy(state.entries[i].registered_events_list);
        }
        state.entries[i].registered_events_list = NULL;
    }
}

bool GDF_EVENT_Register(u16 e_code, void* listener, GDF_EventHandlerFP callback)
{
    if (!INITIALIZED)
        return false;
    
    if (state.entries[e_code].registered_events_list == NULL)
    {
        state.entries[e_code].registered_events_list = GDF_LIST_Create(registered_event);
    }

    // quick check for dupe listeners
    u64 registered_count = GDF_LIST_GetLength(state.entries[e_code].registered_events_list);
    for (u64 i = 0; i < registered_count; i++)
    {
        if (state.entries[e_code].registered_events_list[0].listener == listener)
        {
            LOG_WARN("Tried to add duplicate event listener. Event code: %u", e_code);
            return false;
        }
    }

    registered_event event;
    event.listener = listener;
    event.callback = callback;
    GDF_LIST_Push(state.entries[e_code].registered_events_list, event);
    return true;
}

bool GDF_EVENT_Unregister(u16 e_code, void* listener, GDF_EventHandlerFP callback)
{
    if (!INITIALIZED)
        return false;
    
    if (state.entries[e_code].registered_events_list == NULL)
    {
        LOG_WARN("Tried to unregister event listener, but there were no listeners. Event code: %u", e_code);
        return false;
    }

    u64 registered_count = GDF_LIST_GetLength(state.entries[e_code].registered_events_list);
    for (u64 i = 0; i < registered_count; i++)
    {
        registered_event* e = &state.entries[e_code].registered_events_list[i];
        if (e->listener == listener && e->callback == callback)
        {
            registered_event removed_event;
            GDF_LIST_Pop(state.entries[e_code].registered_events_list, &removed_event);
            return true;
        }
    }

    LOG_WARN("Couldn't find event listener to unregister. Event code: %u", e_code);
    return false;
}

bool GDF_EVENT_Fire(u16 e_code, void* sender, GDF_EventCtx ctx)
{
    if (!INITIALIZED)
        return false;
    
    if (state.entries[e_code].registered_events_list == NULL)
    {
        return true;
    }

    u64 registered_count = GDF_LIST_GetLength(state.entries[e_code].registered_events_list);
    for (u64 i = 0; i < registered_count; i++)
    {
        registered_event* e = &state.entries[e_code].registered_events_list[i];
        if (e->callback(e_code, sender, e->listener, ctx))
        {
            // just a way to make it so the event cancels if it
            // returns true, useful for stuff potentially
            // but i dont like it so ill prob change it later
            return true;
        }
    }
}