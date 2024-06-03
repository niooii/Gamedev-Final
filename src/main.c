#ifndef COMPILE_BUILDER

#include "game/game.h"
#include "core/os/info.h"
#include "app.h"
#include "core/asserts.h"
#include "core/subsystems.h"

int main()
{
    GDF_InitSubsystems(GDF_SUBSYSTEM_WINDOWING | GDF_SUBSYSTEM_EVENTS | GDF_SUBSYSTEM_INPUT | GDF_SUBSYSTEM_NET);
    GDF_InitApp();
    f64 time_ran_for = GDF_RunApp();
    if (time_ran_for != -1)
    {
        LOG_INFO("App has been running for %lf seconds... Time to rest!", time_ran_for);
    }
    GDF_ShutdownSubsystems();
    return 0;
}

#endif