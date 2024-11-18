#pragma once

#include "core.h"
#include "engine/math/math.h"

typedef struct Transform {
    vec3 pos;
    // Rotation is always in radians.
    vec3 rot;
    vec3 scale;
    mat4 model_matrix;
} Transform;

FORCEINLINE void transform_recalc_model_matrix(Transform* transform)
{
    transform->model_matrix = mat4_mul(
        mat4_mul(
            mat4_scale(transform->scale), 
            mat4_rotation(transform->rot)
        ),
        mat4_translation(transform->pos)
    );
}

FORCEINLINE void transform_init_default(Transform* out_transform)
{
    out_transform->scale = vec3_new(1,1,1);
    transform_recalc_model_matrix(out_transform);
}
