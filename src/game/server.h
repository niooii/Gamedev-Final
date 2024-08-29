#pragma once
#include "core.h"
#include "engine/core/os/socket.h"
#include "world/world.h"

// NOT MINECRFT CLONE I SWEAR
#define SERVER_PORT 25567

typedef enum WORLDSERVER_EXIT_CODE {
    GRACEFUL,
    SOCKET_ERR,
} WORLDSERVER_EXIT_CODE;

typedef struct WorldServer {
    World world;
    GDF_Socket* client_list;
} WorldServer;

WORLDSERVER_EXIT_CODE world_server_run();