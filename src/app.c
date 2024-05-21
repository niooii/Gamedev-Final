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
        if (!GDF_AppSettings_Save())
        {
            LOG_ERR("Something went wrong saving app settings...");
        }
    }
}

bool GDF_RunApp() 
{
    if (!INITIALIZED)
    {
        LOG_ERR("You didnt initialize the app yet. dipshit. you're a bad person, yk that?");
        return false;
    }
    // LOG_INFO("allocating 10m values on the custom heap...");
    // for (int i = 0; i < 10000000; i++)
    // {
    //     void* block = GDF_Malloc(sizeof(int), GDF_MEMTAG_APPLICATION);
    //     GDF_Free(block);
    // }
    // LOG_INFO("done");
    // LOG_INFO("allocating 10m values with malloc...");
    // for (int i = 0; i < 10000000; i++)
    // {
    //     void* block = malloc(sizeof(int));
    //     free(block);
    // }
    // LOG_INFO("done");
    // char* buf = GDF_Malloc(8000 * sizeof(char), GDF_MEMTAG_STRING);
    // GDF_GetMemUsageStr(buf);
    // LOG_INFO("%s", buf);
    while(APP_STATE.is_running) 
    {
        GDF_PumpMessages();
    }

    return true;
}