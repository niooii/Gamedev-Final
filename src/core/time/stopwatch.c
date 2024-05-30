#include "stopwatch.h"
#include "core/os/info.h"

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
    stopwatch->start_time = GDF_GetAbsoluteTime();
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