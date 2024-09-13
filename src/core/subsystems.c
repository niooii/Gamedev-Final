#include "subsystems.h"

static u32 _flags;

bool GDF_InitSubsystems(u32 flags)
{
    _flags = flags;
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

bool GDF_ShutdownSubsystems()
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

    return true;
}