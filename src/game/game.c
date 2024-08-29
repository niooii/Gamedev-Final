#include "game.h"
#include "engine/core/input.h"
#include "engine/math/math.h"
#include "engine/math/math_types.h"

static GDF_Game* GAME;

bool GDF_GAME_Init()
{
    GAME = GDF_Malloc(sizeof(*GAME), GDF_MEMTAG_GAME);
    GAME->main_camera = GDF_Malloc(sizeof(GDF_Camera), GDF_MEMTAG_GAME);
    camera_init_default(GAME->main_camera);

    // TODO! uncomment later, the game will be initilaized in world state for now.
    // GAME->current_screen = GDF_GAME_SCREEN_MAIN_MENU;
    // GAME->current_screen_type = GDF_GAME_SCREENTYPE_GUI_MENU;
    // GAME->main_player = NULL;
    // GAME->world = NULL;

    GAME->current_screen = GDF_GAME_SCREEN_IN_WORLD;
    GAME->current_screen_type = GDF_GAME_SCREENTYPE_WORLD;
    GAME->main_player = NULL;
    GAME->world = GDF_Malloc(sizeof(World), GDF_MEMTAG_GAME);
    WorldCreateInfo world_info = {
        .chunk_simulate_distance = 16,
        .ticks_per_sec = 20,
    };
    world_create(GAME->world, &world_info);
    return true;
}

bool GDF_GAME_Update(f32 dt)
{
    // quick camera input test stuff 
    // TODO! remove
    f32 move_speed = 3;
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_LCONTROL))
    {
        move_speed = 20;
    }
    GDF_Camera* camera = GAME->main_camera;
    vec3 camera_forward = vec3_forward(camera->yaw  * DEG_TO_RAD, camera->pitch * DEG_TO_RAD);
    vec3 camera_right = vec3_right_from_forward(camera_forward);

    vec3 right_vec = vec3_new(camera_right.x, 0, camera_right.z);
    vec3_normalize(&right_vec);
    vec3 forward_vec = vec3_new(camera_forward.x, 0, camera_forward.z);
    vec3_normalize(&forward_vec);
    // TODO! handle normalization when traveling diagonally
    right_vec = vec3_mul_scalar(right_vec, move_speed * dt);
    forward_vec = vec3_mul_scalar(forward_vec, move_speed * dt);

    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_W))
    {
        vec3_add_to(&camera->pos, forward_vec);
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_S))
    {
        vec3_add_to(&camera->pos, vec3_negated(forward_vec));
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_A))
    {
        vec3_add_to(&camera->pos, vec3_negated(right_vec));
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_D))
    {
        vec3_add_to(&camera->pos, right_vec);
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_SPACE))
    {
        camera->pos.y += move_speed * dt;
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_LSHIFT))
    {
        camera->pos.y -= move_speed * dt;
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_UP))
    {
        camera->pitch -= dt;
        LOG_DEBUG("TURNING CAMERA YES %f", camera->pitch);
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_DOWN))
    {
        camera->pitch += dt;
        LOG_DEBUG("TURNING CAMERA YES %f", camera->pitch);
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_LEFT))
    {
        camera->yaw += dt;
        LOG_DEBUG("TURNING CAMERA YES %f", camera->yaw);
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_RIGHT))
    {
        camera->yaw -= dt;
        LOG_DEBUG("TURNING CAMERA YES %f", camera->yaw);
    }
    i32 dx;
    i32 dy;
    GDF_INPUT_GetMouseDelta(&dx, &dy);
    camera->yaw -= dx * 0.4;
    camera->pitch -= dy * 0.4;
    // TODO! weird behavior when not clamped: when pitch passes -90 or 90, scene flips??
    camera->pitch = CLAMP(camera->pitch, -89, 89);
    camera_recalc_view_matrix(camera);
    return true;
}

GDF_Game* GDF_GAME_GetInstance() 
{
    return GAME;
}