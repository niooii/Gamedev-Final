#include "stopwatch.h"

static inline f64 timespec_to_seconds(struct timespec* ts)
{
    return (f64)(ts->tv_sec) + ts->tv_nsec/1000000000.0;
}

static inline void reset_stopwatch(GDF_Stopwatch* stopwatch)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    stopwatch->prev = timespec_to_seconds(&ts);
}

GDF_Stopwatch* GDF_CreateStopwatch()
{
    GDF_Stopwatch* stopwatch = GDF_Malloc(sizeof(*stopwatch), GDF_MEMTAG_UNKNOWN);
    reset_stopwatch(stopwatch);
    return stopwatch;
}

// nanosecond presicion (kinda)
f64 GDF_StopwatchTimeElapsed(GDF_Stopwatch* stopwatch)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return timespec_to_seconds(&ts) - stopwatch->prev;
}
f64 GDF_StopwatchReset(GDF_Stopwatch* stopwatch)
{
    reset_stopwatch(stopwatch);
}
void GDF_DestroyStopwatch(GDF_Stopwatch* stopwatch)
{
    
}