#pragma once

#include "core.h"
#include "engine/math/math.h"
#include "game/entity/entity.h"
#include "game/world/world_types.h"
#include "game/world/block.h"

typedef enum RAYCAST_STATUS {
    RAYCAST_STATUS_ERR_UNKNOWN,
    RAYCAST_STATUS_HIT,
    RAYCAST_STATUS_ERR_NO_HIT,
    RAYCAST_STATUS_ERR_NO_DIRECTION
} RAYCAST_STATUS;

typedef struct RaycastInfo {
    vec3 origin;
    vec3 dir;
    f32 max_distance;
    World* world;
} RaycastInfo;

// Returns a recommended RaycastInfo struct for general block raycasting.
FORCEINLINE RaycastInfo raycast_info_new(
    World* world,
    vec3 origin, 
    vec3 dir,
    f32 max_distance
)
{
    return (RaycastInfo) {
        .origin = origin,
        .dir = dir,
        .max_distance = max_distance,
        .world = world
    };
}

typedef struct RaycastBlockHitInfo {
    // If this is not a RAYCAST_STATUS_ERR status, then it is safe to get information
    // from other fields.
    RAYCAST_STATUS status;

    Block* block;
    vec3 block_world_pos;
    Chunk* chunk;
    ChunkCoord chunk_coord;
    WORLD_DIRECTION direction;
    BLOCK_FACE hit_face;
} RaycastBlockHitInfo;

typedef struct RaycastEntityHitInfo {
    // If this is not a RAYCAST_STATUS_ERR status, then it is safe to get information
    // from other fields.
    RAYCAST_STATUS status;

    Entity* entity;
} RaycastEntityHitInfo;

void raycast_blocks(RaycastInfo* info, RaycastBlockHitInfo* result);
RaycastEntityHitInfo raycast_entity(RaycastInfo* info);