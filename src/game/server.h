#pragma once
#include "core.h"
#include "engine/core/os/socket.h"
#include "engine/core/os/thread.h"
#include "world/world.h"

// NOT MINECRFT CLONE I SWEAR
#define SERVER_PORT 25567

typedef enum WORLDSERVER_EXIT_CODE {
    GRACEFUL,
    SOCKET_ERR,
    WORLD_CREATION_ERR,
    WORLD_LOAD_ERR,
    SYNC_ERR
} WORLDSERVER_EXIT_CODE;

typedef enum PACKET_TYPE {
    PACKET_HANDSHAKE,
    PACKET_DISCONNECT,
    PACKET_PLAYER_POSITION,
    PACKET_BLOCK_UPDATE
    // etc etc
} PACKET_TYPE;

typedef struct WorldServer {
    World world;
    GDF_Mutex client_sockets_mutex;
    GDF_Socket* client_list;
    bool alive;
} WorldServer;

// World path must be a valid path to a world or NULL, if NULL then create_info must not be NULL.
typedef struct WorldServerStartInfo {
    const char* world_path;
    WorldCreateInfo* create_info;
} WorldServerStartInfo;

// WorldServerStartInfo must be configured correctly to either
WORLDSERVER_EXIT_CODE world_server_run(WorldServerStartInfo* start_info, WorldServer* ctx);