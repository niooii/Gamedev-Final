#include "movement.h"

void jump(HumanoidEntity* humanoid, f32 jump_power)
{
    PhysicsComponent* physics = humanoid->base.physics;
    physics->vel.y = 7 * jump_power;
}

// Set to 1 for max
const f32 AIR_CONTROL = 0.5;
// How significantly a player is moving in a direction 
const f32 AIR_CONTROL_THRESHOLD = 0.1f;

void player_apply_movement(
    HumanoidEntity* humanoid, 
    i8 x_input,
    i8 z_input,
    vec3* forward,
    vec3* right,
    f32 dt,
    bool just_jumped,
    f32 speed
)
{
    PhysicsComponent* physics = humanoid->base.physics;

    vec3 dv = {0};

    vec3 right_vec = vec3_new(right->x, 0, right->z);
    vec3_normalize(&right_vec);
    vec3 forward_vec = vec3_new(forward->x, 0, forward->z);
    vec3_normalize(&forward_vec);

    f32 true_speed = speed;
    if (just_jumped)
        true_speed = speed * 4;
    else if (!physics->grounded)
        true_speed = speed * 0.3;

    vec3_add_to(&dv, vec3_mul_scalar(forward_vec, z_input));
    vec3_add_to(&dv, vec3_mul_scalar(right_vec, x_input));
    
    if (x_input != 0 || z_input != 0) 
    {
        vec3_normalize(&dv);
    }

    dv = vec3_mul_scalar(dv, true_speed);

    // vec3 horizontal_vel = vec3_new(physics->vel.x, 0, physics->vel.z);
    // f32 current_speed = vec3_length(horizontal_vel);

    // if (!physics->grounded) {
    //     vec3 vel_dir = horizontal_vel;
    //     if (current_speed > AIR_CONTROL_THRESHOLD) {
    //         vec3_normalize(&vel_dir);
            
    //         f32 movement_alignment = vec3_dot(vel_dir, dv);

    //         // LOG_DEBUG("DOT PRODUCT: %f", movement_alignment);
            
    //         // Allows for change in velocity very close to 180 degs
    //         if (movement_alignment < 0.1f) {
    //             return;
    //         }
    //     }
        
    //     // Apply air control only when changing direction or moving slowly
    //     if (!just_jumped)
    //         dv = vec3_mul_scalar(dv, AIR_CONTROL);
    // }

    vec3_add_to(&physics->vel, dv);
}

void dash(
    HumanoidEntity* humanoid, 
    f32 dash_power,
    vec3 forward
)
{
    vec3 dash_vec = vec3_mul_scalar(forward, dash_power * 40);
    dash_vec.y *= 0.4;
    vec3_add_to(&humanoid->base.physics->vel, dash_vec);
    humanoid->in_dash = true;
}