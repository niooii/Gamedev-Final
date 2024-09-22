#pragma once

#include "core.h"
#include "os/sysinfo.h"
#include "os/io.h"
#include "os/window.h"
#include "os/socket.h"
#include "core/event.h"
#include "core/input.h"

enum {
    GDF_SUBSYSTEM_WINDOWING = 0b00000001,
    GDF_SUBSYSTEM_EVENTS =    0b00000010,
    GDF_SUBSYSTEM_INPUT =     0b00000100,
    GDF_SUBSYSTEM_NET =       0b00001000,
};

// womp womp
bool GDF_InitSubsystems(u32 flags);

bool GDF_ShutdownSubsystems();