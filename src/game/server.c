#include "server.h"

WORLDSERVER_EXIT_CODE listen_for_connections(WorldServer* ctx)
{
    GDF_Socket server_socket = GDF_MakeSocket();
    if (!GDF_SocketListen(server_socket, SERVER_PORT))
    {
        return WORLDSERVER_SOCKET_ERR;
    }

    GDF_Socket client_sock;
    while((client_sock = GDF_SocketAccept(server_socket)))
    {
        if (!GDF_LockMutex(ctx->clients_mutex))
        {
            return WORLDSERVER_SYNC_ERR;
        }
        // add to list
        // ctx->clients[];
        ctx->connected_clients++;
        LOG_INFO("NEW CLIENT!!");

        GDF_ReleaseMutex(ctx->clients_mutex);
    }

    return WORLDSERVER_GRACEFUL;
}

unsigned long client_accepting_thread_wrapper(void* args)
{
    GDF_InitThreadLogging("ClientListener");
    WorldServer* ctx = (WorldServer*)args;
    listen_for_connections(ctx);
    return 0;
}

bool world_server_init(WorldServerStartInfo* start_info, WorldServer* ctx)
{
    ctx->clients_mutex = GDF_CreateMutex();
    ctx->clients = GDF_LIST_Reserve(ClientInfo, ctx->max_clients);
    // TODO! load world properly
    WorldCreateInfo world_create_info = {
        .ticks_per_sec = 5,
        .chunk_simulate_distance = 16,
    };  
    world_create(&ctx->world, &world_create_info);

    ctx->initialized = true;
    return true;
};

WORLDSERVER_EXIT_CODE world_server_run(WorldServer* ctx)
{
    if (!ctx->initialized)
    {
        LOG_ERR("Server has not been initialized correctly - exit.");
        return WORLDSERVER_NOT_INITIALIZED;
    }
    GDF_Thread client_accepting_thread = GDF_CreateThread(client_accepting_thread_wrapper, ctx);
    ctx->alive = true;

    while (ctx->alive)                             
    {
        World* world = &ctx->world;
        GDF_StopwatchReset(world->world_update_stopwatch);
        // Broadcast tick event
        // TODO! packet serialization

        // TODO! heavy processing stuff
        int a;
        for (int i = 0; i < 4800000; i++)
        {
            a++;
        }

        // Updating world finish, wait for next tick
        f64 t = GDF_StopwatchElasped(world->world_update_stopwatch);
        f64 period = 1.f/(world->ticks_per_sec);
        if (t < period)
        {
            GDF_ThreadSleep(1000.f * (period - t));
        }
        else
        {
            LOG_WARN("Server fell behind by %0.3lfs...", t - period);
        }
        
        LOG_INFO("UPDATE");
    }

    // TODO! Somehow signal for client accepting thread to end
    GDF_JoinThread(client_accepting_thread);

    return WORLDSERVER_GRACEFUL;
}