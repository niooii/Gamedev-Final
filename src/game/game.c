#include "game.h"
#include "core/input.h"
#include "engine/math/math.h"
#include "engine/math/math_types.h"
#include "engine/render/renderer.h"
#include "movement.h"
#include "engine/math/raycast.h"

static GDF_Game* GAME;
static HumanoidEntity* player;

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
    // GAME->main_player = NULL;
    GAME->world = GDF_Malloc(sizeof(World), GDF_MEMTAG_GAME);
    WorldCreateInfo world_info = {
        .chunk_simulate_distance = 16,
        .ticks_per_sec = 20,
    };
    world_create(GAME->world, &world_info);
    player = world_create_humanoid(GAME->world);
    physics_add_entity(GAME->world->physics, &player->base);
    player->base.aabb.min = vec3_new(-0.375, 0, -0.375);
    player->base.aabb.max = vec3_new(0.375, 1.8, 0.375);
    aabb_translate(&player->base.aabb, vec3_new(1, 5, 1));
    player->base.health = 100;
    player->base.damagable = true;

    return true;
}

// TODO!
bool GDF_GAME_Update(f32 dt)
{
    // quick camera input test stuff 
    // TODO! remove
    f32 move_speed = 1;
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_LCONTROL))
    {
        move_speed = 1.5;
    }
    GDF_Camera* camera = GAME->main_camera;
    vec3 camera_forward = vec3_forward(camera->yaw  * DEG_TO_RAD, camera->pitch * DEG_TO_RAD);
    vec3 camera_right = vec3_right_from_forward(camera_forward);

    f32 scale_factor = dt * 10;
    i8 z_input = 0;
    i8 x_input = 0;
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_W))
    {
        z_input++;
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_S))
    {
        z_input--;
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_A))
    {
        x_input--;
    }
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_D))
    {
        x_input++;
    }
    if (GDF_INPUT_IsKeyPressed(GDF_KEYCODE_Q))
    {
        dash(player, 1.f, camera_forward);
    }
    bool jumped = false;
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_SPACE) && player->base.grounded)
    {
        jump(player, 1);
        jumped = true;
    }
    player_apply_movement(
        player,
        x_input,
        z_input,
        &camera_forward,
        &camera_right,
        dt,
        jumped,
        move_speed
    );
    if (GDF_INPUT_IsKeyDown(GDF_KEYCODE_LSHIFT))
    {
        player->base.vel.y = -move_speed;
    }
    if (GDF_INPUT_IsKeyPressed(GDF_KEYCODE_V))
    {
        Renderer* renderer = renderer_get_instance();
        renderer->render_mode = !renderer->render_mode;
    }
    // LOG_INFO("VEL: %f %f %f", player_comp->vel.x, player_comp->vel.y, player_comp->vel.z);
    // cam is in center of players head 
    camera->pos.x = (player->base.aabb.min.x + player->base.aabb.max.x) / 2.0;
    camera->pos.y = player->base.aabb.max.y - 0.5;
    camera->pos.z = (player->base.aabb.min.z + player->base.aabb.max.z) / 2.0;
    i32 dx;
    i32 dy;
    GDF_INPUT_GetMouseDelta(&dx, &dy);
    camera->yaw += dx * 0.1;
    camera->pitch -= dy * 0.1;
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

    // TODO! make a isbuttonpressed like iskeypressed
    if (GDF_INPUT_IsButtonDown(GDF_MBUTTON_RIGHT))
    {
        LOG_INFO("RAYING..");
        RaycastInfo raycast_info = raycast_info_new(
            GAME->world,
            camera->pos,
            camera_forward,
            4
        );
        RaycastBlockHitInfo result;
        raycast_blocks(&raycast_info, &result);
        if (result.status == RAYCAST_STATUS_HIT) 
        {
            LOG_DEBUG("HIT BLOCK LETS GIO");
            u8 x_dif=0, y_dif=0, z_dif=0;
            switch (result.hit_face)
            {
                case BLOCK_FACE_BOT:
                    y_dif = -1;
                    break;
                case BLOCK_FACE_TOP:
                    y_dif = 1;
                    break;
                case BLOCK_FACE_BACK:
                    z_dif = -1;
                    break;
                case BLOCK_FACE_FRONT:
                    z_dif = 1;
                    break;
                case BLOCK_FACE_LEFT:
                    x_dif = -1;
                    break;
                case BLOCK_FACE_RIGHT:
                    x_dif = 1;
                    break;
            }
            vec3 place_pos = vec3_new(
                result.block_world_pos.x + x_dif,
                result.block_world_pos.y + y_dif,
                result.block_world_pos.z + z_dif
            );
            BlockCreateInfo info = {
                .type = GDF_BLOCKTYPE_Grass,
                .world_pos = place_pos
            };
            world_set_block(result.chunk, &info);
        }
    }
    // LOG_DEBUG("pos: %f %f %f", player->base.aabb.min.x, player->base.aabb.min.y, player->base.aabb.min.z);
    // LOG_DEBUG("vel: %f %f %f", player_comp->vel.x, player_comp->vel.y, player_comp->vel.z);
    return true;//
}

GDF_Game* GDF_GAME_GetInstance() 
{
    return GAME;
}