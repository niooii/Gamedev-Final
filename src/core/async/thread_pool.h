#pragma once

#include "core.h"

typedef struct GDF_Threadpool_T* GDF_Threadpool;

GDF_Threadpool GDF_CreateThreadpool();
void GDF_ThreadpoolSubmit(GDF_Threadpool pool, u64);
