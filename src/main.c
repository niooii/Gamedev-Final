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
        while(1);
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
    // GDF_InitApp();
    // f64 time_ran_for = GDF_RunApp();
    // if (time_ran_for != -1)
    // {
    //     LOG_INFO("App has been running for %lf seconds... Time to rest!", time_ran_for);
    // }
    GDF_ShutdownSubsystems();
    return 0;
}

#endif