#pragma once

#include "core.h"
#include "os/info.h"
#include "os/io.h"
#include "os/window.h"
#include "os/socket.h"
#include "core/event.h"

enum {
    GDF_SUBSYSTEM_WINDOWING = 0b00000001,
    GDF_SUBSYSTEM_EVENTS =    0b00000010,
};

static u32 _flags;

// womp womp
inline bool GDF_InitSubsystems(u32 flags)
{
    _flags = flags;
    if (!GDF_InitMemory())
        return false;
    if (!GDF_InitLogging())
        return false;
    GDF_InitIO();
    if (!GDF_InitInfo())
        return false;
    if (flags & GDF_SUBSYSTEM_WINDOWING)
    {
        if (!GDF_InitWindowing())
            return false;
    }
    if (flags & GDF_SUBSYSTEM_EVENTS)
    {
        if (!GDF_InitEvents())
            return false;
    }
    return true; 
}

inline bool GDF_ShutdownSubsystems()
{
    if (_flags & GDF_SUBSYSTEM_EVENTS)
    {
        GDF_ShutdownEvents();
    }
    if (_flags & GDF_SUBSYSTEM_WINDOWING)
    {
        GDF_ShutdownWindowing();
    }
    GDF_ShutdownLogging();

    // should come last or something
    GDF_ShutdownMemory();
}