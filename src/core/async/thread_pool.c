#include "thread_pool.h"
#include "core/os/thread.h"
#include "core/collections/list.h"

#define WORK_Q_PREALLOC_SIZE 256

typedef struct tp_work {

    GDF_Semaphore finished_semaphore;
} tp_work;

typedef struct GDF_Threadpool_T {
    u16 workers;
    GDF_LIST(tp_work) work_queue;
} GDF_Threadpool_T;

GDF_Threadpool GDF_CreateThreadpool()
{
    GDF_Threadpool tp = GDF_Malloc(sizeof(GDF_Threadpool_T), GDF_MEMTAG_APPLICATION);
    tp->work_queue = GDF_LIST_Reserve(tp_work, WORK_Q_PREALLOC_SIZE);

    return tp;
}