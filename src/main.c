#ifndef COMPILE_BUILDER

#include "game/game.h"
#include "engine/core/os/misc.h"
#include "app.h"
#include "engine/core/asserts.h"
#include "engine/core/subsystems.h"
#include "engine/core/os/thread.h"
#include "game/server.h"

int test_counter = 0;
GDF_Mutex test_counter_mutex;
unsigned long thread_test_callback(void* args) 
{
    u32 thread_num = *((u32*)args);
    LOG_WARN("hi from thread %u", thread_num);
    for (int i = 0; i < 1000000; i++)
    {
        GDF_LockMutex(test_counter_mutex);
        test_counter++;
        GDF_ReleaseMutex(test_counter_mutex);
    }
    LOG_WARN("bye from thread %u", thread_num);
    return 0;
}

unsigned long server_thread_wrapper(void* args)
{
    world_server_run();
    return 0;
}

int main()
{
    GDF_InitSubsystems(GDF_SUBSYSTEM_WINDOWING | GDF_SUBSYSTEM_EVENTS | GDF_SUBSYSTEM_INPUT | GDF_SUBSYSTEM_NET);
    // GDF_Socket* serverSocket = GDF_MakeSocket();
    // if (!serverSocket) {
    //     printf("Failed to create server socket.\n");
    //     GDF_ShutdownSockets();
    //     return -1;
    // }

    // // Listen on port 8080
    // if (!GDF_SocketListen(serverSocket, 8080)) {
    //     printf("Failed to listen on port 8080.\n");
    //     GDF_DestroySocket(serverSocket);
    //     GDF_ShutdownSockets();
    //     return -1;
    // }

    // printf("Server listening on port 8080...\n");
    GDF_CreateThread(server_thread_wrapper, NULL);
    GDF_Socket* client = GDF_MakeSocket();

    if (!GDF_SocketConnect(client, "127.0.0.1", SERVER_PORT))
    {
        LOG_ERR("FALIED TO CONNECT");
    }
    else
    {
        LOG_INFO("CONECTED\n");
    }
    return 0;

    // GDF_ChunkCubeFace face = {
    //     .data = 0
    // };

    // u64 flags = 0;
    // SET_BITS(flags, 6, 10, 128);
    // SET_BITS(flags, 16, 6, 22);
    // SET_BITS(flags, 60, 4, 14);
    // u64 new3 = GET_BITS(flags, 60, 4);
    // u64 new1 = GET_BITS(flags, 6, 10);
    // u64 new2 = GET_BITS(flags, 16, 6);
    // // unsigned int mask=1<<((sizeof(u64)<<3)-1);
    // printf("%u\n", new1);
    // printf("%u\n", new2);
    // printf("%u\n", new3);

    // // TODO! test bit ops
    // return -1;
    test_counter_mutex = GDF_CreateMutex();
    GDF_Thread handles[3];
    for (u32 i = 0; i < 3; i++)
    {
        handles[i] = GDF_CreateThread(thread_test_callback, &i);
    }
    for (u32 i = 0; i < 3; i++)
    {
        GDF_JoinThread(handles[i]);
    }
    printf("COUNTER IS NOW: %d, SHOULD BE 3000000", test_counter);
    // GDF_InitApp();
    // f64 time_ran_for = GDF_RunApp();
    // if (time_ran_for != -1)
    // {
    //     LOG_INFO("App has been running for %lf seconds... Time to rest!", time_ran_for);
    // }
    // GDF_ShutdownSubsystems();
    return 0;
}

#endif