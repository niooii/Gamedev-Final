#pragma once

#include "core.h"
#include "os/info.h"
#include "os/io.h"
#include "os/window.h"
#include "os/socket.h"
#include "engine/core/event.h"
#include "engine/core/input.h"

enum {
    GDF_SUBSYSTEM_WINDOWING = 0b00000001,
    GDF_SUBSYSTEM_EVENTS =    0b00000010,
    GDF_SUBSYSTEM_INPUT =     0b00000100,
    GDF_SUBSYSTEM_NET =       0b00001000,
};

static u32 _flags;

// womp womp
static bool GDF_InitSubsystems(u32 flags)
{
    _flags = flags;
    if (!GDF_InitMemory())
        return false;
    if (!GDF_InitLogging())
        return false;
    GDF_InitIO();
    if (!GDF_InitInfo())
        return false;
    if ((flags & GDF_SUBSYSTEM_WINDOWING) == GDF_SUBSYSTEM_WINDOWING)
    {
        if (!GDF_InitWindowing())
            return false;
    }
    if ((flags & GDF_SUBSYSTEM_EVENTS) == GDF_SUBSYSTEM_EVENTS)
    {
        if (!GDF_InitEvents())
            return false;
    }
    if ((flags & GDF_SUBSYSTEM_INPUT) == GDF_SUBSYSTEM_INPUT)
    {
        // if the events subsystem wasn't initialized return false w an error
        if ((flags & GDF_SUBSYSTEM_EVENTS) != GDF_SUBSYSTEM_EVENTS)
        {
            LOG_ERR("Input subsystem depends on events subsystem.");
            return false;
        }
        GDF_InitInput();
    }
    if ((flags & GDF_SUBSYSTEM_NET) == GDF_SUBSYSTEM_NET)
    {
        if (!GDF_InitSockets())
            return false;
    }
    return true; 
}

static bool GDF_ShutdownSubsystems()
{
    if (_flags & GDF_SUBSYSTEM_EVENTS)
    {
        GDF_ShutdownEvents();
    }
    if (_flags & GDF_SUBSYSTEM_WINDOWING)
    {
        GDF_ShutdownWindowing();
    }
    if (_flags & GDF_SUBSYSTEM_INPUT)
    {
        GDF_ShutdownInput();
    }
    GDF_ShutdownLogging();

    // should come last or something
    GDF_ShutdownMemory();

    return true;
}