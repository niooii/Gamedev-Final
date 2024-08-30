#ifndef COMPILE_BUILDER

#include "game/game.h"
#include "engine/core/os/misc.h"
#include "app.h"
#include "engine/core/asserts.h"
#include "engine/core/subsystems.h"
#include "engine/core/os/thread.h"
#include "game/server.h"

unsigned long server_thread_wrapper(void* args)
{
    WorldServer ctx;
    WorldServerStartInfo start_info = {
        .max_clients = 20,
    };
    world_server_init(&start_info, &ctx);
    world_server_run(&ctx);
    return 0;
}

int main()
{
    GDF_InitSubsystems(GDF_SUBSYSTEM_WINDOWING | GDF_SUBSYSTEM_EVENTS | GDF_SUBSYSTEM_INPUT | GDF_SUBSYSTEM_NET);
    
    // TODO! eventually move to either a dedicated server start or creating a world. 
    GDF_Thread server_thread = GDF_CreateThread(server_thread_wrapper, NULL);
    GDF_Socket client = GDF_MakeSocket();

    if (!GDF_SocketConnect(client, "127.0.0.1", SERVER_PORT))
    {
        LOG_ERR("FALIED TO CONNECT");
    }
    else
    {
        printf("CONNECTED!!!!\n");
    }

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