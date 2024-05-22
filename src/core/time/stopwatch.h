#pragma once

#include "core.h"
#include <time.h>

typedef struct GDF_Stopwatch {
    f64 prev;
} GDF_Stopwatch;

GDF_Stopwatch* GDF_CreateStopwatch();
// nanosecond presicion (kinda)
f64 GDF_StopwatchTimeElapsed(GDF_Stopwatch* stopwatch);
f64 GDF_StopwatchReset(GDF_Stopwatch* stopwatch);
void GDF_DestroyStopwatch(GDF_Stopwatch* stopwatch);