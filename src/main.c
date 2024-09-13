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

int main()
{
    if (!GDF_InitMemory() || !GDF_InitLogging() || !GDF_InitThreadLogging("Main"))
        return false;
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
    // test carr impl
    GDF_CArray arr = GDF_CArrayCreate(f64, 2000000);
    GDF_Stopwatch* benchmarker = GDF_Stopwatch_Create();
    u32 r_iterations = 0;
    for (int i = 0; i < 2000000; i++)
    {
        f64* entry = GDF_CArrayWriteNext(arr);
        *entry = i;
    }
    LOG_INFO("write ops took %lf seconds..", GDF_Stopwatch_TimeElasped(benchmarker));

    GDF_Stopwatch_Reset(benchmarker);
    int sum;
    for (const f64* fp = GDF_CArrayReadNext(arr); fp != NULL; fp = GDF_CArrayReadNext(arr))
    {
        r_iterations++;
        sum+=*fp;
    }
    LOG_INFO("read ops took %lf seconds..", GDF_Stopwatch_TimeElasped(benchmarker));
    LOG_INFO("read iters: %u", r_iterations);
    return 1;
    // test map impl
    GDF_HashMap map = GDF_HashmapCreate(int, int, false);
    int key1 = 24;
    int val1 = 200400;
    GDF_ASSERT(GDF_HashmapInsert(map, &key1, &val1));
    int* val1_p = GDF_HashmapGet(map, &key1);
    LOG_INFO("got val1: %d", *val1_p);
    int key2 = 223434;
    int val2 = 343435;
    GDF_ASSERT(GDF_HashmapInsert(map, &key2, &val2));
    int* val2_p = GDF_HashmapGet(map, &key2);
    // should fail, duplicate key
    GDF_ASSERT(!GDF_HashmapInsert(map, &key2, &val1));
    LOG_INFO("got val2: %d", *val2_p);

    // key iteration test
    HashMapEntry* iter1 = GDF_HashmapIter(map);
    GDF_ASSERT(iter1 != NULL)
    for (; iter1 != NULL; GDF_HashmapIterNext(&iter1))
    {
        LOG_INFO("1 key: %d, val: %d", *((int*)iter1->key), *((int*)iter1->val));
    }
    GDF_HashmapRemove(map, &key1, NULL);
    HashMapEntry* iter2 = GDF_HashmapIter(map);
    GDF_ASSERT(iter2 != NULL)
    for (; iter2 != NULL; GDF_HashmapIterNext(&iter2))
    {
        LOG_INFO("2 key: %d, val: %d", *((int*)iter2->key), *((int*)iter2->val));
    }
    LOG_INFO("tests finish");

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