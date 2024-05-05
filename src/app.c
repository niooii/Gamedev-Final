#include "app.h"

typedef struct AppState {
    bool is_running;
    bool is_suspended;
    i16 width;
    i16 height;
    f64 last_time;
} AppState;

static bool initialized = false;
static AppState state;
static GDF_Window* main_window;

bool GDF_InitApp(GDF_AppConfig* config) 
{
    if (initialized) 
    {
        LOG_ERR("sooo we're initilaizing multiple times");
        return false;
    }

    init_logging();

    state.is_running = true;
    state.is_suspended = false;

    initialized = true;

    main_window = GDF_CreateWindow(config->spawn_x, config->spawn_y, config->spawn_w, config->spawn_h, config->window_name);
    return true;
}

bool GDF_Run() 
{
    if (!initialized)
    {
        LOG_ERR("You didnt initialize the app yet. dipshit. you're a bad person, yk that?");
        return false;
    }
    while(state.is_running) 
    {
        GDF_PumpMessages();
    }

    return true;
}