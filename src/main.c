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
#include <time.h>

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
    LOG_INFO("hi chat");
    GDF_InitSubsystems(GDF_SUBSYSTEM_WINDOWING | GDF_SUBSYSTEM_EVENTS | GDF_SUBSYSTEM_INPUT | GDF_SUBSYSTEM_NET);

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
    // test physics impl
    // PhysicsCreateInfo info = {
    //     .gravity = {0, -9.81, 0},
    //     .gravity_active = true
    // };
    // PhysicsEngine physics = physics_init(info);
    // f64 time = GDF_GetAbsoluteTime();
    // PhysicsComponent* comp = physics_create_component(physics);
    // while (true)
    // {
    //     f64 dt = GDF_GetAbsoluteTime() - time;
    //     time = GDF_GetAbsoluteTime();
    //     physics_update(physics, dt);
    //     LOG_DEBUG("pos: %f, %f, %f", comp->pos.x, comp->pos.y, comp->pos.z);
    // }
    // test carr impl
    // GDF_CArray arr = GDF_CArrayCreate(f64, 2000000);
    // GDF_Stopwatch benchmarker = GDF_StopwatchCreate();
    // u32 r_iterations = 0;
    // for (int i = 0; i < 2000000; i++)
    // {
    //     f64* entry = GDF_CArrayWriteNext(arr);
    //     LOG_WARN("%d", i);
    //     *entry = i;
    // }
    // LOG_INFO("write ops took %lf seconds..", GDF_StopwatchElasped(benchmarker));

    // GDF_StopwatchReset(benchmarker);
    // int sum;
    // for (const f64* fp = GDF_CArrayReadNext(arr); fp != NULL; fp = GDF_CArrayReadNext(arr))
    // {
    //     r_iterations++;
    //     sum+=*fp;
    // }
    // LOG_INFO("read ops took %lf seconds..", GDF_StopwatchElasped(benchmarker));
    // LOG_INFO("read iters: %u", r_iterations);
    // return 1;
    // test map impl
    // GDF_HashMap map = GDF_HashmapCreate(int, int, false);
    // int key1 = 24;
    // int val1 = 200400;
    // GDF_ASSERT(GDF_HashmapInsert(map, &key1, &val1));
    // int* val1_p = GDF_HashmapGet(map, &key1);
    // LOG_INFO("got val1: %d", *val1_p);
    // int key2 = 223434;
    // int val2 = 343435;
    // GDF_ASSERT(GDF_HashmapInsert(map, &key2, &val2));
    // int* val2_p = GDF_HashmapGet(map, &key2);
    // // // should fail, duplicate key
    // GDF_ASSERT(!GDF_HashmapInsert(map, &key2, &val1));
    // LOG_INFO("got val2: %d", *val2_p);

    // // key iteration test
    // HashmapEntry* iter1 = GDF_HashmapIter(map);
    // GDF_ASSERT(iter1 != NULL)
    // LOG_WARN("Testing first iteration (2 elements)...");
    // for (; iter1 != NULL; GDF_HashmapIterNext(&iter1))
    // {
    //     LOG_INFO("key: %d, val: %d", *((int*)iter1->key), *((int*)iter1->val));
    // }
    // srand(time(NULL));
    // GDF_HashmapRemove(map, &key1, NULL);
    // for (u32 i = 1; i < 3300; i++)
    // {   
    //     int v = rand() % 200;
    //     GDF_HashmapInsert(map, &i, &v);
    // }
    // HashmapEntry* iter2 = GDF_HashmapIter(map);
    // GDF_ASSERT(iter2 != NULL)
    // LOG_WARN("Testing second iteration (1 element)...");
    // for (; iter2 != NULL; GDF_HashmapIterNext(&iter2))
    // {
    //     LOG_INFO("key: %d, val: %d", *((int*)iter2->key), *((int*)iter2->val));
    // }
    // LOG_INFO("tests finish");
    // GDF_ThreadSleep(1000);
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