#ifndef COMPILE_BUILDER

#include "game/game.h"
#include "core/os/misc.h"
#include "app.h"
#include "core/asserts.h"
#include "core/subsystems.h"
#include "core/os/thread.h"
#include "game/server.h"
#include "core/collections/hashmap.h"

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
    // GDF_Thread server_thread = GDF_CreateThread(server_thread_wrapper, NULL);
    // GDF_Socket client = GDF_MakeSocket();

    // if (!GDF_SocketConnect(client, "127.0.0.1", SERVER_PORT))
    // {
    //     LOG_ERR("FALIED TO CONNECT");
    // }
    // else
    // {
    //     printf("CONNECTED!!!!\n");
    // }

    // test map impl
    GDF_HashMap test_map = GDF_HashmapCreate(int, int, false);
    int testval1 = 200400;
    int testkey1 = 24;
    GDF_HashmapInsert(test_map, &testkey1, &testval1);
    int* testval1_p = GDF_HashmapGet(test_map, &testkey1);
    printf("got val: %d\n", *testval1_p);
    return 1;

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