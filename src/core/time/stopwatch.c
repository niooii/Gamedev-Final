#include "stopwatch.h"
#include "core/os/sysinfo.h"

GDF_Stopwatch* GDF_Stopwatch_Create()
{
    GDF_Stopwatch* stopwatch = GDF_Malloc(sizeof(*stopwatch), GDF_MEMTAG_APPLICATION);
    stopwatch->start_time = GDF_GetAbsoluteTime();
    return stopwatch;
}

f64 GDF_Stopwatch_TimeElasped(GDF_Stopwatch* stopwatch)
{
    return GDF_GetAbsoluteTime() - stopwatch->start_time;
}

f64 GDF_Stopwatch_Reset(GDF_Stopwatch* stopwatch)
{
    f64 curr = GDF_GetAbsoluteTime();
    f64 elapsed = curr - stopwatch->start_time;
    stopwatch->start_time = curr;
    return elapsed;
}

void GDF_Stopwatch_Pause(GDF_Stopwatch* stopwatch)
{
    
}

void GDF_Stopwatch_Resume(GDF_Stopwatch* stopwatch)
{

}

void GDF_Stopwatch_Destroy(GDF_Stopwatch* stopwatch)
{
    GDF_Free(stopwatch);
}