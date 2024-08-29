#pragma once

#include "core.h"
#include "engine/math/math.h"

typedef struct Transform {
    vec3 pos;
    // Rotation is always in radians.
    vec3 rot;
    vec3 scale;
    mat4 model_matrix;
} GDF_Transform;

FORCEINLINE void GDF_TRANSFORM_RecalculateModelMatrix(GDF_Transform* transform)
{
    transform->model_matrix = mat4_mul(
        mat4_mul(
            mat4_scale(transform->scale), 
            mat4_rotation(transform->rot)
        ),
        mat4_translation(transform->pos)
    );
}

FORCEINLINE void GDF_TRANSFORM_InitDefault(GDF_Transform* out_transform)
{
    out_transform->scale = vec3_new(1,1,1);
    GDF_TRANSFORM_RecalculateModelMatrix(out_transform);
}
