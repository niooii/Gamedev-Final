#pragma once

#include "core.h"
#include "os/info.h"
#include "os/io.h"
#include "os/heap.h"
#include "os/window.h"
#include "os/socket.h"

// womp womp
inline bool GDF_InitSubsystems()
{
    if (!GDF_InitMemory())
        return false;
    if (!GDF_InitLogging())
        return false;
    GDF_InitIO();
    if (!GDF_InitInfo())
        return false;
    if (!GDF_InitWindowing())
        return false;
    return true; 
}

inline bool GDF_ShutdownSubsystems()
{
    GDF_ShutdownLogging();
    // should come last or something
    GDF_ShutdownMemory();
}