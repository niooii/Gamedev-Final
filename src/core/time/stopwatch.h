#pragma once

#include "core.h"

typedef struct GDF_Stopwatch_T* GDF_Stopwatch;

// Stopwatch is created in "resumed" state.
GDF_Stopwatch GDF_StopwatchCreate();
// in seconds
f64 GDF_StopwatchElasped(GDF_Stopwatch stopwatch);
f64 GDF_StopwatchReset(GDF_Stopwatch stopwatch);
// TODO!
void GDF_StopwatchPause(GDF_Stopwatch stopwatch);
// TODO!
void GDF_StopwatchResume(GDF_Stopwatch stopwatch);
void GDF_StopwatchDestroy(GDF_Stopwatch stopwatch);