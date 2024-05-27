#ifndef COMPILE_BUILDER

#include "game/game.h"
#include "core/os/info.h"
#include "app.h"
#include "core/asserts.h"
#include "core/subsystems.h"

int main()
{
    GDF_InitSubsystems(GDF_SUBSYSTEM_WINDOWING | GDF_SUBSYSTEM_EVENTS);
    GDF_InitApp();
    GDF_RunApp();
    return 0;
}

#endif