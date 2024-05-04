#pragma once
#include "def.h"
#include "logging.h"
#include "os/window.h"

typedef struct app_config 
{
    i16 spawn_x;
    i16 spawn_y;
    i16 spawn_w;
    i16 spawn_h;

    char* window_name;
} app_config;


GDFAPI bool app_create(app_config* config);

GDFAPI bool app_run();