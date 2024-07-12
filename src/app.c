#include "app.h"
#include "engine/core/containers/list.h"
#include "engine/core/input.h"
#include "engine/math/math.h"

typedef struct AppState {
    bool is_running;
    // bool is_suspended;
    i16 width;
    i16 height;
    f64 last_time;
    GDF_Stopwatch* stopwatch;
} AppState;

static bool INITIALIZED = false;
static AppState APP_STATE;
static GDF_Window* MAIN_WINDOW;
static bool mouse_lock_toggle = false;

// is gonna be registered for a bunch of events
bool app_on_event(u16 event_code, void *sender, void *listener_instance, GDF_EventContext ctx)
{
    switch (event_code)
    {
        case GDF_EVENT_INTERNAL_KEY_PRESSED:
        {
            u16 key_code = ctx.data.u16[0];
            if (key_code == GDF_KEYCODE_ESCAPE)
            {
                GDF_EventContext tmp_ctx = {.data = 0};
                GDF_EVENT_Fire(GDF_EVENT_INTERNAL_APP_QUIT, NULL, tmp_ctx);
                return true;
            }
            switch (key_code) {
                case GDF_KEYCODE_GRAVE:
                {
                    mouse_lock_toggle = !mouse_lock_toggle;
                    GDF_CURSOR_LOCK_STATE state = mouse_lock_toggle ? GDF_CURSOR_LOCK_STATE_Locked : GDF_CURSOR_LOCK_STATE_Free;
                    GDF_INPUT_SetMouseLockState(state);
                    LOG_DEBUG("TOGGLE MOUSE LOCK");
                }
            }
            break;
        }
        case GDF_EVENT_INTERNAL_WINDOW_RESIZE:
        {
            u16 width = ctx.data.u16[0];
            u16 height = ctx.data.u16[1];

            // Check if different. If so, trigger a resize event.
            u16 old_w;
            u16 old_h;
            GDF_GetWindowSize(&old_w, &old_h);
            if (width != old_w || height != old_h) 
            {
                GDF_SetWindowSizeInternal(width, height);

                LOG_DEBUG("Window resize: %i, %i", width, height);

                // Handle minimization
                if (width == 0 || height == 0) 
                {
                    LOG_INFO("Window is minimized kinda.");
                }
                GDF_RENDERER_Resize(width, height);
            }
            return false;
        }
        case GDF_EVENT_INTERNAL_APP_QUIT:
        {
            LOG_INFO("Shutting down app...");
            APP_STATE.is_running = false;
            // dont return true as other listeners may want to destroy resources
            // on app quit
            return false;
        }
    }

    return false;
}

#include "engine/camera.h"
static Camera camera;
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
    GDF_EVENT_Register(GDF_EVENT_INTERNAL_WINDOW_RESIZE, NULL, app_on_event);

    // initialize the renderer
    if (!GDF_InitRenderer(GDF_RENDER_BACKEND_TYPE_VULKAN))
    {
        LOG_ERR("Couldn't initialize renderer unlucky.");
        return false;
    }
    camera.pos = vec3_new(0, 0, -2);
    GDF_CAMERA_InitDefault(&camera);
    GDF_RENDERER_SetCamera(&camera);

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
    u32 fps = 2222;
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

        GDF_RenderPacket packet;
        packet.delta_time = dt;
        GDF_RENDERER_DrawFrame(&packet);

        f64 frame_time = GDF_Stopwatch_TimeElasped(frame_timer);
        
        // wait a certain amount of time after each frame to cap fps
        // TODO! add uncap fps option probably "bool uncap_fps" or some shit
        f64 wait_secs = secs_per_frame - frame_time;
        if (wait_secs > 0)
        {
            GDF_Sleep((u64)(wait_secs * 1000));
        }

        // quick camera input test stuff 
        // TODO! remove
        vec3 camera_forward = vec3_forward(camera.yaw * DEG_TO_RAD, camera.pitch * DEG_TO_RAD);
        vec3 camera_right = vec3_right_from_forward(camera_forward);
        vec3 camera_up = vec3_cross(camera_forward, camera_right);

        camera_forward = vec3_mul_scalar(camera_forward, 2 * dt);
        camera_right = vec3_mul_scalar(camera_right, 2 * dt);
        camera_up = vec3_mul_scalar(camera_up, 2 * dt);
        if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_W))
        {
            vec3_add_to(&camera.pos, camera_forward);
        }
        if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_S))
        {
            vec3_add_to(&camera.pos, vec3_negated(camera_forward));
        }
        if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_A))
        {
            vec3_add_to(&camera.pos, vec3_negated(camera_right));
        }
        if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_D))
        {
            vec3_add_to(&camera.pos, camera_right);
        }
        if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_SPACE))
        {
            camera.pos.y += dt;
        }
        if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_LSHIFT))
        {
            camera.pos.y -= dt;
        }
        if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_UP))
        {
            camera.pitch -= dt;
            LOG_DEBUG("TURNING CAMERA YES %f", camera.pitch);
        }
        if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_DOWN))
        {
            camera.pitch += dt;
            LOG_DEBUG("TURNING CAMERA YES %f", camera.pitch);
        }
        if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_LEFT))
        {
            camera.yaw += dt;
            LOG_DEBUG("TURNING CAMERA YES %f", camera.yaw);
        }
        if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_RIGHT))
        {
            camera.yaw -= dt;
            LOG_DEBUG("TURNING CAMERA YES %f", camera.yaw);
        }
        i32 dx;
        i32 dy;
        GDF_INPUT_GetMouseDelta(&dx, &dy);
        camera.yaw -= dx * 0.4;
        camera.pitch -= dy * 0.4;
        // TODO! weird behavior when not clamped: when pitch passes -90 or 90, scene flips??
        camera.pitch = CLAMP(camera.pitch, -89, 89);
        GDF_CAMERA_RecalculateViewMatrix(&camera);
        // LOG_DEBUG("curr: %d | prev: %d", mouse_x, prev_mouse_x);

        GDF_INPUT_Update(dt);

        // only thing that should produce innacuracies is if pumpmessages takes a bit of time
        APP_STATE.last_time = current_time;
    }

    // CLEAN UP STUFF
    f64 time_ran_for = GDF_Stopwatch_TimeElasped(running_timer);
    // destroy resources
    GDF_Stopwatch_Destroy(APP_STATE.stopwatch);
    GDF_Stopwatch_Destroy(running_timer);

    GDF_DestroyRenderer();

    return time_ran_for;
}