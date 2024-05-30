#pragma once

#include "core.h"

typedef struct GDF_Stopwatch {
    f64 start_time;
} GDF_Stopwatch;

// Stopwatch is created in "resumed" state.
GDF_Stopwatch* GDF_Stopwatch_Create();
// in seconds
f64 GDF_Stopwatch_TimeElasped(GDF_Stopwatch* stopwatch);
f64 GDF_Stopwatch_Reset(GDF_Stopwatch* stopwatch);
// TODO!
void GDF_Stopwatch_Pause(GDF_Stopwatch* stopwatch);
// TODO!
void GDF_Stopwatch_Resume(GDF_Stopwatch* stopwatch);
void GDF_Stopwatch_Destroy(GDF_Stopwatch* stopwatch);