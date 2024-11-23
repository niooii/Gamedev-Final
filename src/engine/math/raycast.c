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

RaycastResult raycast(RaycastInfo* info)
{
    RaycastResult result = {0};

    vec3 ray_pos = info->origin;
    if (vec3_length_squared(info->dir) == 0)
    {
        result.status = RAYCAST_STATUS_ERR_NO_DIRECTION;
        return result;
    }
    vec3 dir = vec3_normalized(info->dir);
    vec3 step_vec = vec3_mul_scalar(dir, info->step_distance);

    for (
        f32 dist_traveled = 0; 
        dist_traveled < info->max_distance; 
        dist_traveled += info->step_distance
    )
    {
        if (info->mode == RAYCAST_MODE_ENTITIES || info->mode == RAYCAST_MODE_WHATEVER)
        {
            
        }
        if (info->mode == RAYCAST_MODE_BLOCKS || info->mode == RAYCAST_MODE_WHATEVER)
        {
            RaycastBlockHitInfo ret_info = {0};
            if (__check_blocks(info->world, ray_pos, &info))
            {
                result.block_info = ret_info;
                result.status = RAYCAST_STATUS_HIT_BLOCK;
                return result;
            }
        }
        vec3_add_to(&ray_pos, step_vec);
    }

    // one last case at max_distance
    ray_pos = vec3_add(info->origin, vec3_mul_scalar(dir, info->max_distance));
    if (info->mode == RAYCAST_MODE_ENTITIES || info->mode == RAYCAST_MODE_WHATEVER)
    {
        TODO("Entity raycasting");
    }
    if (info->mode == RAYCAST_MODE_BLOCKS || info->mode == RAYCAST_MODE_WHATEVER)
    {
        RaycastBlockHitInfo ret_info = {0};
        if (__check_blocks(info->world, ray_pos, &info))
        {
            result.block_info = ret_info;
            result.status = RAYCAST_STATUS_HIT_BLOCK;
            return result;
        }
    }

    result.status = RAYCAST_STATUS_ERR_NO_HIT;
    return result;
}