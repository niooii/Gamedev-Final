#ifndef COMPILE_BUILDER

#include "game/game.h"
#include "engine/core/os/info.h"
#include "app.h"
#include "engine/core/asserts.h"
#include "engine/core/subsystems.h"

int main()
{
    GDF_InitSubsystems(GDF_SUBSYSTEM_WINDOWING | GDF_SUBSYSTEM_EVENTS | GDF_SUBSYSTEM_INPUT | GDF_SUBSYSTEM_NET);
    // u64 yes = 0b10000000111;
    // printf("val unshifted: %lu\n", yes);
    // printf("val u8 shifted: %u\n", (yes >> 8));
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
    // GDF_Socket* client = GDF_MakeSocket();

    // if (!GDF_SocketConnect(client, "127.0.0.1", 8080))
    // {
    //     LOG_ERR("FALIED TO CONNECT");
    // }
    // else
    // {
    //     printf("CONECTED\n");
    // }
    // return 0;
    
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