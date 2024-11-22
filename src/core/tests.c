#include "tests.h"
#include <time.h>

static GDF_Stopwatch benchmarker;

void test_hashmap();
void test_carr();
void test_math();

void run_all_tests()
{
    benchmarker = GDF_StopwatchCreate();
    test_math();
    test_hashmap();
    // test_carr();
}

void test_hashmap() 
{
    LOG_DEBUG("Testing hashmap impl");
    GDF_HashMap map = GDF_HashmapCreate(int, int, false);
    int key1 = 24;
    int val1 = 200400;
    GDF_ASSERT(GDF_HashmapInsert(map, &key1, &val1));
    int* val1_p = GDF_HashmapGet(map, &key1);
    LOG_DEBUG("got val1: %d", *val1_p);
    int key2 = 223434;
    int val2 = 343435;
    GDF_ASSERT(GDF_HashmapInsert(map, &key2, &val2));
    int* val2_p = GDF_HashmapGet(map, &key2);
    // // should fail, duplicate key
    GDF_ASSERT(!GDF_HashmapInsert(map, &key2, &val1));
    LOG_DEBUG("got val2: %d", *val2_p);

    // key iteration test
    HashmapEntry* iter1 = GDF_HashmapIter(map);
    GDF_ASSERT(iter1 != NULL)
    LOG_WARN("Testing first iteration (2 elements)...");
    for (; iter1 != NULL; GDF_HashmapIterAdvance(&iter1))
    {
        LOG_DEBUG("key: %d, val: %d", *((int*)iter1->key), *((int*)iter1->val));
    }
    srand(time(NULL));
    GDF_HashmapRemove(map, &key1, NULL);
    for (u32 i = 1; i < 3300; i++)
    {   
        int v = rand() % 200;
        GDF_HashmapInsert(map, &i, &v);
    }
    HashmapEntry* iter2 = GDF_HashmapIter(map);
    GDF_ASSERT(iter2 != NULL)
    LOG_WARN("Testing second iteration (1 element)...");
    for (; iter2 != NULL; GDF_HashmapIterAdvance(&iter2))
    {
        LOG_DEBUG("key: %d, val: %d", *((int*)iter2->key), *((int*)iter2->val));
    }
    LOG_DEBUG("Hashmap tests finish");
}

void test_carr()
{
    GDF_CArray arr = GDF_CArrayCreate(f64, 4000);
    GDF_Stopwatch benchmarker = GDF_StopwatchCreate();
    u32 r_iterations = 0;
    for (int i = 0; i < 8000; i++)
    {
        f64* entry = GDF_CArrayWriteNext(arr);
        *entry = i;
    }
    LOG_INFO("write ops took %lf seconds..", GDF_StopwatchElasped(benchmarker));

    GDF_StopwatchReset(benchmarker);
    int sum;
    for (const f64* fp = GDF_CArrayReadNext(arr); fp != NULL; fp = GDF_CArrayReadNext(arr))
    {
        r_iterations++;
        LOG_WARN("%f", fp);
        sum+=*fp;
    }
    LOG_INFO("read ops took %lf seconds..", GDF_StopwatchElasped(benchmarker));
    LOG_INFO("read iters: %u", r_iterations);
}

void test_math()
{
    GDF_ASSERT(FLOOR(-0.00001) == -1);
    GDF_ASSERT(FLOOR(-16.45 / 4) == -5);
}