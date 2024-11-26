#pragma once

#include "core.h"
#include "engine/physics/physics.h"

typedef struct HumanoidEntity HumanoidEntity;

// 1 is default
void jump(HumanoidEntity* humanoid, f32 jump_power);

void player_apply_movement(
    HumanoidEntity* humanoid, 
    i8 x_input,
    i8 z_input,
    vec3* forward,
    vec3* right,
    f32 dt,
    bool just_jumped,
    f32 speed
);

typedef struct MovementState {
    bool can_dash;
    bool in_dash;
    vec3 dash_dir;
} MovementState;

void dash(
    HumanoidEntity* humanoid, 
    f32 dash_power,
    vec3 forward
);

void movement_update(HumanoidEntity* hum);