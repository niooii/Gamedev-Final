#pragma once

#include "core.h"
#include "engine/math/math_types.h"
#include "math/math.h"

typedef struct Camera {
    vec3 pos;
    // IN DEGREES
    f32 pitch;
    // IN DEGREES
    f32 yaw;

    mat4 view_matrix;
    mat4 perspective_matrix;
    // view_matrix * perspective_matrix
    mat4 view_perspective;

    f32 aspect_ratio;
    f32 fov;
    f32 near_clip;
    f32 far_clip;
} Camera;

static FORCEINLINE void __recalc_view_perspective(Camera* camera)
{
    camera->view_perspective = mat4_mul(camera->view_matrix, camera->perspective_matrix);
}

FORCEINLINE void GDF_CAMERA_RecalculateViewMatrix(Camera* camera)
{
    camera->view_matrix = mat4_view(camera->pos, camera->yaw * DEG_TO_RAD, camera->pitch * DEG_TO_RAD);
    __recalc_view_perspective(camera);
}

FORCEINLINE void GDF_CAMERA_RecalculatePerspectiveMatrix(Camera* camera)
{
    camera->perspective_matrix = mat4_perspective(camera->fov, camera->aspect_ratio, camera->near_clip, camera->far_clip);
    __recalc_view_perspective(camera);
}

FORCEINLINE void GDF_CAMERA_RecalculateMatrices(Camera* camera)
{
    camera->view_matrix = mat4_view(camera->pos, camera->yaw * DEG_TO_RAD, camera->pitch * DEG_TO_RAD);
    camera->perspective_matrix = mat4_perspective(camera->fov, camera->aspect_ratio, camera->near_clip, camera->far_clip);
    __recalc_view_perspective(camera);
}

FORCEINLINE void GDF_CAMERA_InitDefault(Camera* out_camera)
{
    out_camera->aspect_ratio = 1.77777f;
    out_camera->fov = 50 * DEG_TO_RAD;
    out_camera->near_clip = 0.01f;
    out_camera->far_clip = 1000;

    GDF_CAMERA_RecalculateMatrices(out_camera);
}

FORCEINLINE void GDF_CAMERA_InitWithPerspective(Camera* out_camera, f32 aspect_ratio, f32 fov, f32 near_clip, f32 far_clip)
{
    out_camera->aspect_ratio = aspect_ratio;
    out_camera->fov = fov;
    out_camera->near_clip = near_clip;
    out_camera->far_clip = far_clip;

    GDF_CAMERA_RecalculateMatrices(out_camera);
}

FORCEINLINE void GDF_CAMERA_InitWithPosition(Camera* out_camera, vec3 pos)
{
    out_camera->pos = pos;

    GDF_CAMERA_RecalculateMatrices(out_camera);
}