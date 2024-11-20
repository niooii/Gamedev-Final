#ifndef COMPILE_BUILDER

#include "game/game.h"
#include "core/os/sysinfo.h"
#include "app.h"
#include "core/asserts.h"
#include "core/subsystems.h"
#include "core/os/thread.h"
#include "game/server.h"
#include "core/collections/hashmap.h"
#include "core/collections/carr.h"
#include <stdio.h>
#include "core/tests.h"

unsigned long server_thread_wrapper(void* args)
{
    GDF_InitThreadLogging("Server");
    WorldServer ctx;
    WorldServerStartInfo start_info = {
        .max_clients = 20,
    };
    world_server_init(&start_info, &ctx);
    world_server_run(&ctx);
    return 0;
}

static GDF_Semaphore tsm;

int main()
{
    if (!GDF_InitMemory())
        return false;
    if (!GDF_InitLogging() || !GDF_InitThreadLogging("Main"))
        return false;
    GDF_InitSubsystems(GDF_SUBSYSTEM_WINDOWING | GDF_SUBSYSTEM_EVENTS | GDF_SUBSYSTEM_INPUT | GDF_SUBSYSTEM_NET);
    // GDF_Stopwatch logging_test = GDF_StopwatchCreate();
    // for (int i = 0; i < 50000; i++)
    // {
    //     printf(" %d ", i);
    // }
    // printf("\n");
    // f32 printf_time = GDF_StopwatchElasped(logging_test);
    // GDF_StopwatchReset(logging_test);
    // for (int i = 0; i < 50000; i++)
    // {
    //     LOG_INFO("LOGINFO: %d", i);
    // }
    // f32 log_time = GDF_StopwatchElasped(logging_test);
    // LOG_INFO("\nprintf took: %fs\nLOG_INFO took: %fs", printf_time, log_time);
    // logging_flush_buffer();
    // return 1;

    run_all_tests();

    // TODO! eventually move to either a dedicated server start or creating a world. 
    GDF_Thread server_thread = GDF_CreateThread(server_thread_wrapper, NULL);
    GDF_Socket client = GDF_MakeSocket();

    if (!GDF_SocketConnect(client, "127.0.0.1", SERVER_PORT))
    {
        LOG_ERR("FALIED TO CONNECT");
    }
    else
    {
        LOG_INFO("CONNECTED!!!!\n");
    }

    if (!GDF_InitApp())
    {
        LOG_ERR("gg this is tragic....\n");
    }
    f64 time_ran_for = GDF_RunApp();
    if (time_ran_for != -1)
    {
        LOG_INFO("App has been running for %lf seconds... Time to rest!", time_ran_for);
    }
    else
    {
        LOG_ERR("yikes....\n");
    }
    logging_flush_buffer();
    GDF_ShutdownSubsystems();
    return 0;
}

#endif