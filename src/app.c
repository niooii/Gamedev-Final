#include "app.h"
#include "core/containers/list.h"
#include "core/input.h"

typedef struct AppState {
    bool is_running;
    bool is_suspended;
    i16 width;
    i16 height;
    f64 last_time;
    GDF_Stopwatch* stopwatch;
} AppState;

static bool INITIALIZED = false;
static AppState APP_STATE;
static GDF_Window* MAIN_WINDOW;

// is gonna be registered for a bunch of events
bool app_on_event(u16 event_code, void *sender, void *listener_instance, GDF_EventCtx ctx)
{
    switch (event_code)
    {
        case GDF_EVENT_INTERNAL_KEY_PRESSED:
        {
            u16 key_code = ctx.data.u16[0];
            if (key_code == GDF_KEYCODE_ESCAPE)
            {
                GDF_EventCtx tmp_ctx = {.data = 0};
                GDF_EVENT_Fire(GDF_EVENT_INTERNAL_APP_QUIT, NULL, tmp_ctx);
                return true;
            }
            break;
        }
        case GDF_EVENT_INTERNAL_APP_QUIT:
        {
            LOG_INFO("Shutting down app...");
            APP_STATE.is_running = false;
            // dont return true as other listeners may want to destroy resources
            // on app quit
        }
    }

    return false;
}

bool GDF_InitApp() 
{
    if (INITIALIZED) 
    {
        LOG_ERR("sooo we're initilaizing multiple times");
        return false;
    }

    // create required files and directories
    GDF_InitFirstLaunch();

    APP_STATE.is_running = true;
    APP_STATE.is_suspended = false;
    if (!GDF_ClientSettings_Load())
    {
        LOG_FATAL("Failed to load client_settings.gdf, exiting...");
        return false;
    }

    GDF_DisplayInfo display_info = GDF_GetDisplayInfo();
    MAIN_WINDOW = GDF_CreateWindow(GDF_WIN_CENTERED, GDF_WIN_CENTERED, display_info.screen_width * 0.45, display_info.screen_height * 0.5, "A GDF");
    if (MAIN_WINDOW != NULL)
        LOG_INFO("Created window.");
    if (GDF_AppSettings_Get()->client_show_console)
    {
        GDF_ShowConsole();
    }
    else
    {
        GDF_HideConsole();
    }

    GDF_EVENT_Register(GDF_EVENT_INTERNAL_KEY_PRESSED, NULL, app_on_event);
    GDF_EVENT_Register(GDF_EVENT_INTERNAL_APP_QUIT, NULL, app_on_event);

    // initialize the renderer
    if (!GDF_InitRenderer(GDF_RENDER_BACKEND_TYPE_VULKAN))
    {
        LOG_ERR("Couldn't initialize renderer unlucky.");
        return false;
    }

    APP_STATE.stopwatch = GDF_Stopwatch_Create();

    INITIALIZED = true;

    return true;
}

bool GDF_InitFirstLaunch()
{
    // if it succeeds, we didnt have an app_settings before and creates one and then saves the default instance of appsettings
    if (GDF_MakeFile("client_settings.gdf"))
    {
        if (!GDF_ClientSettings_Save())
        {
            LOG_ERR("Something went wrong saving client settings...");
            return false;
        }
    }
    return true;
}

f64 GDF_RunApp() 
{
    if (!INITIALIZED)
    {
        LOG_ERR("App not initialized properly");
        return -1;
    }

    GDF_Stopwatch* running_timer = GDF_Stopwatch_Create();
    u8 frame_count = 0;
    u32 fps = 144;
    f64 secs_per_frame = 1.0/fps;
    GDF_Stopwatch* frame_timer = GDF_Stopwatch_Create();

    while(APP_STATE.is_running) 
    {
        GDF_PumpMessages();
        
        f64 current_time = GDF_Stopwatch_TimeElasped(APP_STATE.stopwatch);
        f64 dt = (current_time - APP_STATE.last_time);
        GDF_Stopwatch_Reset(frame_timer);

        // just testing input
        if (GDF_INPUT_IsButtonDown(GDF_MBUTTON_LEFT))
        {
            // LOG_INFO("heehaw");
        }

        f64 frame_time = GDF_Stopwatch_TimeElasped(frame_timer);
        
        // wait a certain amount of time after each frame to cap fps
        // TODO! add uncap fps option probably "bool uncap_fps" or some shit
        f64 wait_secs = secs_per_frame - frame_time;
        if (wait_secs > 0)
        {
            GDF_Sleep((u64)(wait_secs * 1000));
        }

        GDF_INPUT_Update(dt);
        // only thing that should produce innacuracies is if pumpmessages takes a bit of time
        APP_STATE.last_time = current_time;
    }

    f64 time_ran_for = GDF_Stopwatch_TimeElasped(running_timer);
    // destroy resources
    GDF_Stopwatch_Destroy(APP_STATE.stopwatch);
    GDF_Stopwatch_Destroy(running_timer);

    return time_ran_for;
}