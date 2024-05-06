#pragma once
#include "def.h"
#include "core/logging.h"
#include "core/os/window.h"
#include "core/os/info.h"
#include "core/os/io.h"

typedef struct GDF_AppConfig {
    i16 spawn_x;
    i16 spawn_y;
    i16 spawn_w;
    i16 spawn_h;
    char* window_name;
    bool show_console;
} GDF_AppConfig;

bool GDF_InitApp(GDF_AppConfig* config);

bool GDF_Run();