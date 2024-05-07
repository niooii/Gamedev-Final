#include "app.h"

typedef struct AppState {
    bool is_running;
    bool is_suspended;
    i16 width;
    i16 height;
    f64 last_time;
} AppState;

static bool INITIALIZED = false;
static AppState APP_STATE;
static GDF_Window* MAIN_WINDOW;

bool GDF_InitApp(GDF_AppConfig* config) 
{
    if (INITIALIZED) 
    {
        LOG_ERR("sooo we're initilaizing multiple times");
        return false;
    }

    init_logging();
    GDF_IOInit();

    APP_STATE.is_running = true;
    APP_STATE.is_suspended = false;

    INITIALIZED = true;

    MAIN_WINDOW = GDF_CreateWindow(config->spawn_x, config->spawn_y, config->spawn_w, config->spawn_h, config->window_name);
    if (config->show_console)
    {
        GDF_ShowConsole();
    }
    else
    {
        GDF_HideConsole();
    }
    return true;
}

bool GDF_Run() 
{
    if (!INITIALIZED)
    {
        LOG_ERR("You didnt initialize the app yet. dipshit. you're a bad person, yk that?");
        return false;
    }
    GDF_GetDirStructure("worlds");
    while(APP_STATE.is_running) 
    {
        GDF_PumpMessages();
    }

    return true;
}