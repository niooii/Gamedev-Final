#include "app.h"

typedef struct app_stateaa
{
    bool is_running;
    bool is_suspended;
    i16 width;
    i16 height;
    f64 last_time;
} app_state;

static bool initialized = false;
static app_state state;

bool app_create(app_config* config) 
{
    if (initialized) 
    {
        GDF_ERR("sooo we're initilaizing multiple times");
        return false;
    }

    init_logging();

    state.is_running= true;
    state.is_suspended= false;

    initialized = true;

    GDF_CreateWindow(10, 10, 10, 10, "yeo chat");
    return true;
}

bool app_run() 
{
    while(state.is_running) 
    {
        
    }

    return true;
}