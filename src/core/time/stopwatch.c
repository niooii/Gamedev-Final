#include "stopwatch.h"
#include "core/os/sysinfo.h"

typedef struct GDF_Stopwatch_T  {
    f64 start_time;
} GDF_Stopwatch_T;

GDF_Stopwatch GDF_StopwatchCreate()
{
    GDF_Stopwatch stopwatch = GDF_Malloc(sizeof(*stopwatch), GDF_MEMTAG_APPLICATION);
    stopwatch->start_time = GDF_GetAbsoluteTime();
    return stopwatch;
}

f64 GDF_StopwatchElasped(GDF_Stopwatch stopwatch)
{
    return GDF_GetAbsoluteTime() - stopwatch->start_time;
}

f64 GDF_StopwatchReset(GDF_Stopwatch stopwatch)
{
    f64 curr = GDF_GetAbsoluteTime();
    f64 elapsed = curr - stopwatch->start_time;
    stopwatch->start_time = curr;
    return elapsed;
}

void GDF_StopwatchPause(GDF_Stopwatch stopwatch)
{
    
}

void GDF_StopwatchResume(GDF_Stopwatch stopwatch)
{

}

void GDF_StopwatchDestroy(GDF_Stopwatch stopwatch)
{
    GDF_Free(stopwatch);
}