#include "physics.h"
#include "game/world/world.h"
#include "core/collections/list.h"
#include "game/events.h"

typedef struct Physics_T {
    GDF_LIST(Entity*) entities;
    
    f32 terminal_velocity;
    f32 air_drag;
    f32 ground_drag;
    vec3 gravity;
    bool gravity_active; 
} Physics_T;

PhysicsEngine physics_init(PhysicsCreateInfo create_info)
{
    PhysicsEngine physics = GDF_Malloc(sizeof(Physics_T), GDF_MEMTAG_APPLICATION);
    physics->entities = GDF_LIST_Reserve(Entity*, 32);
    physics->gravity = create_info.gravity;
    physics->gravity_active = create_info.gravity_active;
    physics->air_drag = create_info.air_drag;
    physics->ground_drag = create_info.ground_drag;
    physics->terminal_velocity = create_info.terminal_velocity;

    return physics;
}

void physics_add_entity(PhysicsEngine engine, Entity* entity)
{
    GDF_LIST_Push(engine->entities, entity);
}

bool physics_update(PhysicsEngine engine, World* world, f64 dt)
{   
    // TODO! optimize, look into SIMD
    vec3 effective_gravity = engine->gravity_active ? engine->gravity : vec3_zero();
    vec3 net_accel;
    
    u32 len = GDF_LIST_GetLength(engine->entities);
    for (u32 i = 0; i < len; i++)
    {
        Entity* entity = engine->entities[i];

        f32 drag = entity->grounded ? engine->ground_drag : engine->air_drag;
        LOG_INFO("DT: %f", dt);
        // TODO! this aint quite right..
        if (engine->gravity_active)
        {
            f32 drag_factor = 1.0f - (drag * dt);
            drag_factor = MAX(0, drag_factor);
            entity->vel.x *= drag_factor;
            entity->vel.z *= drag_factor;
        }
        
        net_accel = vec3_add(entity->accel, effective_gravity);
        
        vec3 deltas = vec3_new(
            entity->vel.x * dt + 0.5f * net_accel.x * dt * dt,
            entity->vel.y * dt + 0.5f * net_accel.y * dt * dt,
            entity->vel.z * dt + 0.5f * net_accel.z * dt * dt
        );

        entity->vel.x = entity->vel.x + net_accel.x * dt;
        // cap velocity gain at a terminal velocity
        // TODO! should this be for all axis? (and both y directions?)
        if (entity->vel.y > engine->terminal_velocity)
        {
            f32 t_vy = entity->vel.y + net_accel.y * dt;
            if (t_vy < engine->terminal_velocity)
                entity->vel.y = engine->terminal_velocity;
            else
                entity->vel.y = t_vy;
        }
        entity->vel.z = entity->vel.z + net_accel.z * dt;

        // TODO! eliminate phasing through blocks at high velocities
        // with raycasting

        AxisAlignedBoundingBox t_aabb = entity->aabb;
        aabb_translate(&t_aabb, deltas);
        
        BlockTouchingResult results[64];
        u32 results_len = world_get_blocks_touching(
            world, 
            &t_aabb,
            results,
            sizeof(results) / sizeof(*results)
        );

        // Check for floor (only if translated aabb's y is below the current one)
        bool ground_found = false;
        bool should_check_ground = 
            t_aabb.min.y <= entity->aabb.min.y;
        f32 ground_y = 0;
        if (should_check_ground)
        {
            vec3 corner;
            for (u32 i = 0; i < 4; i++)
            {
                switch (i)
                {
                    case 0:
                    corner = aabb_bot_left(&t_aabb);
                    break;
                    case 1:
                    corner = aabb_bot_left_back(&t_aabb);
                    break;
                    case 2:
                    corner = aabb_bot_right(&t_aabb);
                    break;
                    case 3:
                    corner = aabb_bot_right_back(&t_aabb);
                    break;
                }
                // REPLACE WITH RAYCAST (or just handle slabs well)
                ChunkCoord cc = world_pos_to_chunk_coord(corner);
                Block* block = world_get_block_at(world, corner);
                if (block != NULL)
                {
                    ground_found = true;
                    // without +1 is the coordinate from the bottom left
                    ground_y = cc.y * CHUNK_SIZE_Y + block->y_rel + 1;
                    // set aabb at appropriate y level
                    f32 y_offset = ground_y - t_aabb.min.y;
                    vec3 offset_vec = vec3_new(0, y_offset, 0);
                    aabb_translate(&t_aabb, offset_vec);
                    vec3_add_to(&deltas, offset_vec);
                    entity->vel.y = 0;
                    break;
                }
            }
        }

        GDF_EventContext ctx = {
            .data.u64[0] = (u64)entity
        };
        for (u32 i = 0; i < results_len; i++)
        {
            BlockTouchingResult* r = results + i;
            if (aabb_intersects(&t_aabb, &r->box))
            {
                vec3 resolution = aabb_get_intersection_resolution(&t_aabb, &r->box);
                // discard all blocks on ground level, but still fire event
                // (avoids some weird issues with edge collisions)
                if (ground_found && r->box.max.y == ground_y)
                {
                    GDF_EVENT_Fire(GDF_EVENT_BLOCK_TOUCHED, r->block, ctx);
                    continue;
                }

                vec3_add_to(&deltas, resolution);
                aabb_translate(&t_aabb, resolution);
                // zero velocity and shi
                if (resolution.x != 0)
                {
                    entity->vel.x = 0;
                }
                if (resolution.y != 0)
                {
                    entity->vel.y = 0;
                }
                if (resolution.z != 0) 
                {
                    entity->vel.z = 0;
                }
                GDF_EventContext ctx = {
                    .data.u64[0] = (u64)entity
                };
                GDF_EVENT_Fire(GDF_EVENT_BLOCK_TOUCHED, r->block, ctx);
            }
        }
        // update grounded status
        entity->grounded = ground_found;

        aabb_translate(&entity->aabb, deltas);
    }

    return true;
}

