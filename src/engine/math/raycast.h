#pragma once

#include "core.h"
#include "engine/math/math.h"
#include "game/entity/entity.h"
#include "game/world/world_types.h"
#include "game/world/block.h"

typedef enum RAYCAST_STATUS {
    RAYCAST_STATUS_HIT_BLOCK,
    RAYCAST_STATUS_HIT_ENTITY,
    RAYCAST_STATUS_ERR_NO_HIT,
    RAYCAST_STATUS_ERR_NO_DIRECTION
} RAYCAST_STATUS;

typedef enum RAYCAST_MODE {
    // Will not get both entities and blocks, whatever comes first
    RAYCAST_MODE_WHATEVER,
    // Only look for entities when raycasting
    RAYCAST_MODE_ENTITIES,
    // Only look for blocks when raycasting (There is no air block)
    RAYCAST_MODE_BLOCKS
} RAYCAST_MODE;

typedef struct RaycastInfo {
    vec3 origin;
    vec3 dir;
    RAYCAST_MODE mode;
    f32 step_distance;
    f32 max_distance;
    World* world;
} RaycastInfo;

// Returns a recommended RaycastInfo struct for general block raycasting.
FORCEINLINE RaycastInfo raycast_default_block_info(
    World* world,
    vec3 origin, 
    vec3 dir,
    f32 max_distance
)
{
    return (RaycastInfo) {
        .origin = origin,
        .dir = dir,
        .mode = RAYCAST_MODE_BLOCKS,
        .step_distance = 1.f,
        .max_distance = max_distance,
        .world = world
    };
}

// Returns a recommended RaycastInfo struct for general entity raycasting.
FORCEINLINE RaycastInfo raycast_default_entity_info(
    World* world,
    vec3 origin, 
    vec3 dir,
    f32 max_distance
)
{
    return (RaycastInfo) {
        .origin = origin,
        .dir = dir,
        .mode = RAYCAST_MODE_ENTITIES,
        .step_distance = 0.25f,
        .max_distance = max_distance,
        .world = world
    };
}

typedef struct RaycastBlockHitInfo {
    Block* block;
    Chunk* chunk;
    ChunkCoord chunk_coord;
} RaycastBlockHitInfo;

typedef struct RaycastEntityHitInfo {
    Entity* entity;
} RaycastEntityHitInfo;

typedef struct RaycastResult {
    // If this is not a RAYCAST_STATUS_ERR status, then it is safe to get information
    // from other fields.
    RAYCAST_STATUS status;

    RaycastEntityHitInfo entity_info;
    RaycastBlockHitInfo block_info;
} RaycastResult;

RaycastResult raycast(RaycastInfo* info);