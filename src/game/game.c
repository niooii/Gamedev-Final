#include "game.h"
#include "core/input.h"
#include "engine/math/math.h"
#include "engine/math/math_types.h"

static GDF_Game* GAME;
static PhysicsComponent* player_comp;

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
    player_comp = physics_create_component(GAME->world->physics);
    return true;
}

// TODO!
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
        aabb_translate(&player_comp->aabb, forward_vec);
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_S))
    {
        aabb_translate(&player_comp->aabb, vec3_negated(forward_vec));
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_A))
    {
        aabb_translate(&player_comp->aabb, vec3_negated(right_vec));
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_D))
    {
        aabb_translate(&player_comp->aabb, right_vec);
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_SPACE))
    {
        player_comp->vel.y = move_speed;
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_LSHIFT))
    {
        player_comp->vel.y = -move_speed;
    }
    if (GDF_INPUT_IsKeyPressed(GDF_KEYCODE_SPACE))
    {
        LOG_INFO("PRESSED>...");
    }
    // LOG_INFO("VEL: %f %f %f", player_comp->vel.x, player_comp->vel.y, player_comp->vel.z);
    camera->pos = player_comp->aabb.max;
    i32 dx;
    i32 dy;
    GDF_INPUT_GetMouseDelta(&dx, &dy);
    camera->yaw -= dx * 0.4;
    camera->pitch -= dy * 0.4;
    // wrap around yaw
    if (camera->yaw > 180)
    {
        camera->yaw = -180 + (camera->yaw - 180);
    }
    else if (camera->yaw < -180)
    {
        camera->yaw = 180 + (camera->yaw + 180);
    }
    // TODO! weird behavior when not clamped: when pitch passes -90 or 90, scene flips??
    camera->pitch = CLAMP(camera->pitch, -89, 89);
    camera_recalc_view_matrix(camera);
    world_update(GAME->world, dt);
    // LOG_DEBUG("pos: %f %f %f", player_comp->pos.x, player_comp->pos.y, player_comp->pos.z);
    // LOG_DEBUG("vel: %f %f %f", player_comp->vel.x, player_comp->vel.y, player_comp->vel.z);
    return true;
}

GDF_Game* GDF_GAME_GetInstance() 
{
    return GAME;
}