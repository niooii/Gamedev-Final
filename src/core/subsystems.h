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
    GDF_InitMemory();
    GDF_InitLogging();
    GDF_InitIO();
    GDF_InitInfo();
    GDF_InitWindowing();
}

inline bool GDF_ShutdownSubsystems()
{
    GDF_ShutdownLogging();
    // should come last or something
    GDF_ShutdownMemory();
}