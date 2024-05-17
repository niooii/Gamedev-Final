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

bool GDF_InitApp() 
{
    if (INITIALIZED) 
    {
        LOG_ERR("sooo we're initilaizing multiple times");
        return false;
    }

    // create required files and directory
    GDF_InitFirstLaunch();

    APP_STATE.is_running = true;
    APP_STATE.is_suspended = false;
    if (!GDF_AppSettings_Load())
    {
        LOG_FATAL("Failed to load app_settings.gdf, exiting...");
        return false;
    }

    INITIALIZED = true;

    if (GDF_AppSettings_Get()->app_type == GDF_APPTYPE_CLIENT)
    {
        GDF_DisplayInfo display_info = GDF_GetDisplayInfo();
        MAIN_WINDOW = GDF_CreateWindow(GDF_WIN_CENTERED, GDF_WIN_CENTERED, display_info.screen_width * 0.45, display_info.screen_height * 0.5, "A GDF");
        if (GDF_AppSettings_Get()->client_show_console)
        {
            GDF_ShowConsole();
        }
        else
        {
            GDF_HideConsole();
        }
    }
    else 
    {
        GDF_ShowConsole();
    }
    
    return true;
}

bool GDF_InitFirstLaunch()
{
    // if it succeeds, we didnt have an app_settings before and creates one and then saves the default instance of appsettings
    if (GDF_MakeFile("app_settings.gdf"))
    {
        GDF_AppSettings_Save();
    }
}

bool GDF_RunApp() 
{
    if (!INITIALIZED)
    {
        LOG_ERR("You didnt initialize the app yet. dipshit. you're a bad person, yk that?");
        return false;
    }
    while(APP_STATE.is_running) 
    {
        GDF_PumpMessages();
    }

    return true;
}