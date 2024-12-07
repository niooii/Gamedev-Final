#include "raycast.h"
#include "game/world/world.h"

// LORD save me
// https://gamedev.stackexchange.com/questions/47362/cast-ray-to-select-block-in-voxel-game
FORCEINLINE static bool __check_entities(World* world, vec3 ray_pos, RaycastEntityHitInfo* entity_info)
{
    TODO("check entities");

    return false;
}

FORCEINLINE static bool __check_blocks(World* world, vec3 ray_pos, RaycastBlockHitInfo* block_info)
{
    Block* block = world_get_block_at(world, ray_pos);
    if (block != NULL)
    {
        ChunkCoord cc = world_pos_to_chunk_coord(ray_pos);
        block_info->block = block;
        block_info->chunk_coord = cc;
        block_info->chunk = world_get_or_create_chunk(world, cc);
        return true;
    }
    return false;
}

void raycast_blocks(RaycastInfo* info, RaycastBlockHitInfo* result)
{
    // initialization 
    i8 step_x, step_y, step_z;
    BLOCK_FACE x_int, y_int, z_int;
    if (info->dir.x > 0)
    {
        step_x = 1;
        x_int = BLOCK_FACE_LEFT;
    }
    else if (info->dir.x < 0)
    {
        step_x = -1;
        x_int = BLOCK_FACE_RIGHT;
    }
    else
    {
        step_x = 0;
    }
    if (info->dir.y > 0)
    {
        step_y = 1;
        y_int = BLOCK_FACE_BOT;
    }
    else if (info->dir.y < 0)
    {
        step_y = -1;
        y_int = BLOCK_FACE_TOP;
    }
    else
    {
        step_y = 0;
    }
    if (info->dir.z > 0)
    {
        step_z = 1;
        z_int = BLOCK_FACE_FRONT;
    }
    else if (info->dir.z < 0)
    {
        step_z = -1;
        z_int = BLOCK_FACE_BACK;
    }
    else
    {
        step_z = 0;
    }
    
    f32 max_tx, max_ty, max_tz, dx, dy, dz;
    i64 block_x = FLOOR(info->origin.x);
    i64 block_y = FLOOR(info->origin.y);
    i64 block_z = FLOOR(info->origin.z);

    max_tx = (block_x + (info->dir.x > 0 ? 1 : 0) - info->origin.x) / info->dir.x;
    max_ty = (block_y + (info->dir.y > 0 ? 1 : 0) - info->origin.y) / info->dir.y;
    max_tz = (block_z + (info->dir.z > 0 ? 1 : 0) - info->origin.z) / info->dir.z;

    dx = 1/info->dir.x;
    dy = 1/info->dir.y;
    dz = 1/info->dir.z;

    // actual stepping
    f32 i = 0;
    // not a physical one though, just in grid space
    BLOCK_FACE last_intersection = 0;
    // TODO! set an upperbound idiot
    do
    {
        i++;
        if(max_tx < max_ty) { 
            if(max_tx < max_tz) { 
                block_x += step_x;
                max_tx += dx;
                last_intersection = x_int;
            } else { 
                block_z += step_z;
                max_tz += dz;
                last_intersection = z_int;
            } 
        } else { 
            if(max_ty < max_tz) { 
                block_y += step_y;
                max_ty += dy;
                last_intersection = y_int;
            } else { 
                block_z += step_z;
                max_tz += dz;
                last_intersection = z_int;
            } 
        } 

        // TODO! avoid redundant calculations
        vec3 true_block_pos = vec3_new(block_x, block_y, block_z);
        Block* b = world_get_block_at(
            info->world,
            true_block_pos
        );
        if (b != NULL)
        {
            ChunkCoord cc = world_pos_to_chunk_coord(true_block_pos);
            result->status = RAYCAST_STATUS_HIT;
            result->hit_face = last_intersection;
            result->block = b;
            result->chunk_coord = cc;
            result->block_world_pos = true_block_pos;
            result->chunk = world_get_or_create_chunk(info->world, cc);
            return;
        }

    } while (i <= info->max_distance);

    result->status = RAYCAST_STATUS_ERR_NO_HIT;
}

RaycastEntityHitInfo raycast_entity(RaycastInfo* info)
{
    TODO("entity raycasting");
    // RaycastResult result = {0};

    // vec3 ray_pos = info->origin;
    // if (vec3_length_squared(info->dir) == 0)
    // {
    //     result.status = RAYCAST_STATUS_ERR_NO_DIRECTION;
    //     return result;
    // }
    // vec3 dir = vec3_normalized(info->dir);
    // vec3 step_vec = vec3_mul_scalar(dir, info->step_distance);

    // for (
    //     f32 dist_traveled = 0; 
    //     dist_traveled < info->max_distance; 
    //     dist_traveled += info->step_distance
    // )
    // {
    //     if (info->mode == RAYCAST_MODE_ENTITIES || info->mode == RAYCAST_MODE_WHATEVER)
    //     {
            
    //     }
    //     if (info->mode == RAYCAST_MODE_BLOCKS || info->mode == RAYCAST_MODE_WHATEVER)
    //     {
    //         RaycastBlockHitInfo ret_info = {0};
    //         if (__check_blocks(info->world, ray_pos, &info))
    //         {
    //             result.block_info = ret_info;
    //             result.status = RAYCAST_STATUS_HIT_BLOCK;
    //             return result;
    //         }
    //     }
    //     vec3_add_to(&ray_pos, step_vec);
    // }

    // // one last case at max_distance
    // ray_pos = vec3_add(info->origin, vec3_mul_scalar(dir, info->max_distance));
    // if (info->mode == RAYCAST_MODE_ENTITIES || info->mode == RAYCAST_MODE_WHATEVER)
    // {
    //     TODO("Entity raycasting");
    // }
    // if (info->mode == RAYCAST_MODE_BLOCKS || info->mode == RAYCAST_MODE_WHATEVER)
    // {
    //     RaycastBlockHitInfo ret_info = {0};
    //     if (__check_blocks(info->world, ray_pos, &info))
    //     {
    //         result.block_info = ret_info;
    //         result.status = RAYCAST_STATUS_HIT_BLOCK;
    //         return result;
    //     }
    // }

    // result.status = RAYCAST_STATUS_ERR_NO_HIT;
    // return result;
}
