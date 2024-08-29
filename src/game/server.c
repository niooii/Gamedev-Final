#include "server.h"

void listen_for_connections(WorldServer* ctx)
{
    GDF_Socket* server_socket = GDF_MakeSocket();
    if (!GDF_SocketListen(server_socket, SERVER_PORT))
    {
        return SOCKET_ERR;
    }

    GDF_Socket client_sock;
    while(client_sock = GDF_SocketAccept(server_socket))
    {
        if (!GDF_LockMutex(ctx->client_sockets_mutex))
        {
            return SYNC_ERR;
        }
        // add to list

        GDF_ReleaseMutex(ctx->client_sockets_mutex);
    }
}

u64 client_accepting_thread_wrapper(void* args)
{
    WorldServer* ctx = (WorldServer*)args;
    listen_for_connections(ctx);
}

WORLDSERVER_EXIT_CODE world_server_run(WorldServerStartInfo* start_info, WorldServer* ctx)
{
    GDF_Thread client_accepting_thread = GDF_CreateThread(client_accepting_thread_wrapper, ctx);

    while (ctx->alive)
    {
        // TODO! processing stuff
    }

    // TODO! Somehow signal for client accepting thread to end
    GDF_JoinThread(client_accepting_thread);
}