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

    // create required files and directory
    GDF_CreateRequiredFiles();

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

bool GDF_CreateRequiredFiles()
{
    GDF_MakeFile("settings.gdf");
}

bool GDF_Run() 
{
    if (!INITIALIZED)
    {
        LOG_ERR("You didnt initialize the app yet. dipshit. you're a bad person, yk that?");
        return false;
    }
    // test serialization
    GDF_MKEY_InitLookupTable();
    GDF_Map* map = GDF_CreateMap();
    bool val = true;
    f64 val2 = 23.423;
    GDF_AddMapEntry(map, GDF_MKEY_SETTINGS_DEV_CAN_FLY, &val, GDF_MAP_DTYPE_BOOL);
    GDF_AddMapEntry(map, GDF_MKEY_SETTINGS_DEV_NOCLIP, &val2, GDF_MAP_DTYPE_DOUBLE);
    GDF_AddMapEntry(map, GDF_MKEY_SETTINGS_DEV_DRAW_WIREFRAME, "BRUHHAUHURtest", GDF_MAP_DTYPE_STRING);
    
    GDF_WriteMapToFile(map, "settings.gdf");
    GDF_Map* read_map = GDF_CreateMap();
    GDF_ReadMapFromFile("settings.gdf", read_map);
    while(APP_STATE.is_running) 
    {
        GDF_PumpMessages();
    }

    return true;
}