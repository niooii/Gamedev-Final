#pragma once
#include "def.h"
#include "logging.h"
#include "os/window.h"

typedef struct GDF_AppConfig 
{
    i16 spawn_x;
    i16 spawn_y;
    i16 spawn_w;
    i16 spawn_h;

    char* window_name;
} GDF_AppConfig;


GDFAPI bool app_create(GDF_AppConfig* config);

GDFAPI bool app_run();