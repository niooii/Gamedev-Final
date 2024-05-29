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

typedef struct event_system_state {
    event_code_entry entries[MAX_EVENT_CODES];
} event_system_state;

static bool INITIALIZED = false;
static event_system_state state;

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
        GDF_LIST_Destroy(state.entries[i].registered_events_list);
        state.entries[i].registered_events_list = NULL;
    }
}

bool GDF_EVENT_Register(u16 event_code, void* listener, GDF_EventHandlerFP callback)
{
    if (!INITIALIZED)
        return false;
    registered_event* registered_events = state.entries[event_code].registered_events_list;
    if (registered_events == NULL)
    {
        registered_events = GDF_LIST_Create(registered_event);
    }

    // quick check for dupe listeners
    u64 registered_count = GDF_LIST_GetLength(registered_events);
    for (u64 i = 0; i < registered_count; i++)
    {
        if (registered_events[0].listener == listener)
        {
            LOG_WARN("Tried to add duplicate event listener. Event code: %u", event_code);
            return false;
        }
    }

    registered_event event;
    event.listener = listener;
    event.callback = callback;
    GDF_LIST_Push(registered_events, event);

    return true;
}

bool GDF_EVENT_Unregister(u16 event_code, void* listener, GDF_EventHandlerFP callback)
{
    if (!INITIALIZED)
        return false;

    registered_event* registered_events = state.entries[event_code].registered_events_list;
    
    if (registered_events == NULL)
    {
        LOG_WARN("Tried to unregister event listener, but there were no listeners. Event code: %u", event_code);
        return false;
    }

    u64 registered_count = GDF_LIST_GetLength(registered_events);
    for (u64 i = 0; i < registered_count; i++)
    {
        registered_event* e = &registered_events[i];
        if (e->listener == listener && e->callback == callback)
        {
            registered_event removed_event;
            GDF_LIST_Pop(registered_events, &removed_event);
            return true;
        }
    }

    LOG_WARN("Couldn't find event listener to unregister. Event code: %u", event_code);
    return false;
}

bool GDF_EVENT_Fire(u16 event_code, void* sender, GDF_EventCtx ctx)
{
    if (!INITIALIZED)
        return false;

    registered_event* registered_events = state.entries[event_code].registered_events_list;
    if (registered_events == NULL)
    {
        return true;
    }

    u64 registered_count = GDF_LIST_GetLength(registered_events);
    for (u64 i = 0; i < registered_count; i++)
    {
        registered_event* e = &registered_events[i];
        if (e->callback(event_code, sender, e->listener, ctx))
        {
            // just a way to make it so the event cancels if it
            // returns true, useful for stuff potentially
            // but i dont like it so ill prob change it later
            return true;
        }
    }
}