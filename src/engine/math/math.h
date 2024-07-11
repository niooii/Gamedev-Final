#pragma once

#include "def.h"
#include "engine/core/mem.h"
#include "math_types.h"

#define PI 3.14159265358979323846f
#define PI_2 2.0f * PI
#define HALF_PI 0.5f * PI
#define QUARTER_PI 0.25f * PI
#define ONE_OVER_PI 1.0f / PI
#define ONE_OVER_TWO_PI 1.0f / PI_2
#define SQRT_TWO 1.41421356237309504880f
#define SQRT_THREE 1.73205080756887729352f
#define SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define DEG_TO_RAD PI / 180.0f
#define RAD_TO_DEG 180.0f / PI

// The multiplier to convert seconds to milliseconds.
#define SEC_TO_MS 1000.0f

// The multiplier to convert milliseconds to seconds.
#define MS_TO_SEC 0.001f

// A huge number that should be larger than any valid number used.
#define INFINITY 1e30f

// Smallest positive number where 1.0 + FLOAT_EPSILON != 0
#define FLOAT_EPSILON 1.192092896e-07f

// sin fn
f32 gsin(f32 x);
// cos fn
f32 gcos(f32 x);
// acos fn
f32 gacos(f32 x);
// tan fn
f32 gtan(f32 x);
// sqrt fn
f32 gsqrt(f32 x);
// abs value fn
f32 gabs(f32 x);

i32 random();
i32 random_in_range(i32 min, i32 max);

f32 frandom();
f32 frandom_in_range(f32 min, f32 max);

// ------------------------------------------
// Vector 2
// ------------------------------------------

FORCEINLINE vec2 vec2_new(f32 x, f32 y) 
{
    return (vec2) { x, y };
}

FORCEINLINE vec2 vec2_zero() 
{
    return (vec2) { 0.0f, 0.0f };
}

FORCEINLINE vec2 vec2_one() 
{
    return (vec2) { 1.0f, 1.0f };
}

FORCEINLINE vec2 vec2_up() 
{
    return (vec2) { 0.0f, 1.0f };
}

FORCEINLINE vec2 vec2_down() 
{
    return (vec2) { 0.0f, -1.0f };
}

FORCEINLINE vec2 vec2_left() 
{
    return (vec2) { -1.0f, 0.0f };
}

FORCEINLINE vec2 vec2_right() 
{
    return (vec2) { 1.0f, 0.0f };
}

FORCEINLINE vec2 vec2_add(vec2 vector_0, vec2 vector_1) 
{
    return (vec2) { vector_0.x + vector_1.x, vector_0.y + vector_1.y };
}

FORCEINLINE void vec2_add_to(vec2* vector_0, vec2 vector_1) 
{
    vector_0->x += vector_1.x;
    vector_0->y += vector_1.y;
}

FORCEINLINE vec2 vec2_sub(vec2 vector_0, vec2 vector_1) 
{
    return (vec2) { vector_0.x - vector_1.x, vector_0.y - vector_1.y };
}

FORCEINLINE void vec2_sub_from(vec2* vector_0, vec2 vector_1) 
{
    vector_0->x -= vector_1.x;
    vector_0->y -= vector_1.y;
}

FORCEINLINE vec2 vec2_mul(vec2 vector_0, vec2 vector_1) 
{
    return (vec2) { vector_0.x * vector_1.x, vector_0.y * vector_1.y };
}

FORCEINLINE void vec2_mul_by(vec2* vector_0, vec2 vector_1) 
{
    vector_0->x *= vector_1.x;
    vector_0->y *= vector_1.y;
}

FORCEINLINE vec2 vec2_div(vec2 vector_0, vec2 vector_1) 
{
    return (vec2) { vector_0.x / vector_1.x, vector_0.y / vector_1.y };
}

FORCEINLINE void vec2_div_by(vec2* vector_0, vec2 vector_1) 
{
    vector_0->x /= vector_1.x;
    vector_0->y /= vector_1.y;
}

FORCEINLINE f32 vec2_length_squared(vec2 vector) 
{
    return vector.x * vector.x + vector.y * vector.y;
}

FORCEINLINE f32 vec2_length(vec2 vector) 
{
    return gsqrt(vec2_length_squared(vector));
}

FORCEINLINE void vec2_normalize(vec2* vector) 
{
    const f32 length = vec2_length(*vector);
    vector->x /= length;
    vector->y /= length;
}

FORCEINLINE vec2 vec2_normalized(vec2 vector) 
{
    vec2_normalize(&vector);
    return vector;
}

FORCEINLINE bool vec2_cmp(vec2 vector_0, vec2 vector_1, f32 tolerance) 
{
    if (gabs(vector_0.x - vector_1.x) > tolerance) {
        return false;
    }
    if (gabs(vector_0.y - vector_1.y) > tolerance) {
        return false;
    }
    return true;
}

FORCEINLINE f32 vec2_distance(vec2 vector_0, vec2 vector_1) 
{
    vec2 d = (vec2) { vector_0.x - vector_1.x, vector_0.y - vector_1.y };
    return vec2_length(d);
}

// ------------------------------------------
// Vector 3
// ------------------------------------------

FORCEINLINE vec3 vec3_new(f32 x, f32 y, f32 z) 
{
    return (vec3) { x, y, z };
}

FORCEINLINE vec3 vec3_from_vec4(vec4 vector) 
{
    return (vec3) { vector.x, vector.y, vector.z };
}

FORCEINLINE vec4 vec3_to_vec4(vec3 vector, f32 w) 
{
    return (vec4) { vector.x, vector.y, vector.z, w };
}

FORCEINLINE vec3 vec3_zero() 
{
    return (vec3) { 0.0f, 0.0f, 0.0f };
}

FORCEINLINE vec3 vec3_one() 
{
    return (vec3) { 1.0f, 1.0f, 1.0f };
}

FORCEINLINE vec3 vec3_up() 
{
    return (vec3) { 0.0f, 1.0f, 0.0f };
}

FORCEINLINE vec3 vec3_down() 
{
    return (vec3) { 0.0f, -1.0f, 0.0f };
}

FORCEINLINE vec3 vec3_left() 
{
    return (vec3) { -1.0f, 0.0f, 0.0f };
}

FORCEINLINE vec3 vec3_right() 
{
    return (vec3) { 1.0f, 0.0f, 0.0f };
}

FORCEINLINE vec3 vec3_forward() 
{
    return (vec3) { 0.0f, 0.0f, -1.0f };
}

FORCEINLINE vec3 vec3_back() 
{
    return (vec3) { 0.0f, 0.0f, 1.0f };
}

FORCEINLINE vec3 vec3_add(vec3 vector_0, vec3 vector_1) 
{
    return (vec3) { vector_0.x + vector_1.x, vector_0.y + vector_1.y, vector_0.z + vector_1.z };
}

FORCEINLINE void vec3_add_to(vec3* vector_0, vec3 vector_1) 
{
    vector_0->x += vector_1.x;
    vector_0->y += vector_1.y;
    vector_0->z += vector_1.z;
}

FORCEINLINE vec3 vec3_sub(vec3 vector_0, vec3 vector_1) 
{
    return (vec3) { vector_0.x - vector_1.x, vector_0.y - vector_1.y, vector_0.z - vector_1.z };
}

FORCEINLINE void vec3_sub_from(vec3* vector_0, vec3 vector_1) 
{
    vector_0->x -= vector_1.x;
    vector_0->y -= vector_1.y;
    vector_0->z -= vector_1.z;
}

FORCEINLINE vec3 vec3_mul(vec3 vector_0, vec3 vector_1) 
{
    return (vec3) { vector_0.x * vector_1.x, vector_0.y * vector_1.y, vector_0.z * vector_1.z };
}

FORCEINLINE void vec3_mul_by(vec3* vector_0, vec3 vector_1) 
{
    vector_0->x *= vector_1.x;
    vector_0->y *= vector_1.y;
    vector_0->z *= vector_1.z;
}

FORCEINLINE vec3 vec3_mul_scalar(vec3 vector_0, f32 scalar) 
{
    return (vec3) { vector_0.x * scalar, vector_0.y * scalar, vector_0.z * scalar };
}

FORCEINLINE vec3 vec3_div(vec3 vector_0, vec3 vector_1) 
{
    return (vec3) { vector_0.x / vector_1.x, vector_0.y / vector_1.y, vector_0.z / vector_1.z };
}

FORCEINLINE void vec3_div_by(vec3* vector_0, vec3 vector_1) 
{
    vector_0->x /= vector_1.x;
    vector_0->y /= vector_1.y;
    vector_0->z /= vector_1.z;
}

FORCEINLINE f32 vec3_length_squared(vec3 vector) 
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

FORCEINLINE f32 vec3_length(vec3 vector) 
{
    return gsqrt(vec3_length_squared(vector));
}

FORCEINLINE void vec3_normalize(vec3* vector) 
{
    const f32 length = vec3_length(*vector);
    vector->x /= length;
    vector->y /= length;
    vector->z /= length;
}

FORCEINLINE vec3 vec3_normalized(vec3 vector) 
{
    vec3_normalize(&vector);
    return vector;
}

FORCEINLINE f32 vec3_dot(vec3 vector_0, vec3 vector_1) 
{
    return vector_0.x * vector_1.x + vector_0.y * vector_1.y + vector_0.z * vector_1.z;
}

FORCEINLINE vec3 vec3_cross(vec3 vector_0, vec3 vector_1) 
{
    return (vec3) { vector_0.y * vector_1.z - vector_0.z * vector_1.y, vector_0.z * vector_1.x - vector_0.x * vector_1.z, vector_0.x * vector_1.y - vector_0.y * vector_1.x };
}

FORCEINLINE bool vec3_cmp(vec3 vector_0, vec3 vector_1, f32 tolerance) 
{
    if (gabs(vector_0.x - vector_1.x) > tolerance) {
        return false;
    }
    if (gabs(vector_0.y - vector_1.y) > tolerance) {
        return false;
    }
    if (gabs(vector_0.z - vector_1.z) > tolerance) {
        return false;
    }
    return true;
}

FORCEINLINE f32 vec3_distance(vec3 vector_0, vec3 vector_1) 
{
    vec3 d = (vec3) { vector_0.x - vector_1.x, vector_0.y - vector_1.y, vector_0.z - vector_1.z };
    return vec3_length(d);
}

// ------------------------------------------
// Vector 4
// ------------------------------------------

FORCEINLINE vec4 vec4_new(f32 x, f32 y, f32 z, f32 w) 
{
    return (vec4) { x, y, z, w };
}

FORCEINLINE vec3 vec4_to_vec3(vec4 vector) 
{
    return (vec3) { vector.x, vector.y, vector.z };
}

FORCEINLINE vec4 vec4_from_vec3(vec3 vector, f32 w) 
{
    return (vec4) { vector.x, vector.y, vector.z, w };
}

FORCEINLINE vec4 vec4_zero() 
{
    return (vec4) { 0.0f, 0.0f, 0.0f, 0.0f };
}

FORCEINLINE vec4 vec4_one() 
{
    return (vec4) { 1.0f, 1.0f, 1.0f, 1.0f };
}

FORCEINLINE vec4 vec4_add(vec4 vector_0, vec4 vector_1) 
{
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = vector_0.elements[i] + vector_1.elements[i];
    }
    return result;
}

FORCEINLINE void vec4_add_to(vec4* vector_0, vec4 vector_1) 
{
    for (u64 i = 0; i < 4; ++i) {
        vector_0->elements[i] += vector_1.elements[i];
    }
}

FORCEINLINE vec4 vec4_sub(vec4 vector_0, vec4 vector_1) 
{
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = vector_0.elements[i] - vector_1.elements[i];
    }
    return result;
}

FORCEINLINE void vec4_sub_from(vec4* vector_0, vec4 vector_1) 
{
    for (u64 i = 0; i < 4; ++i) {
        vector_0->elements[i] -= vector_1.elements[i];
    }
}

FORCEINLINE vec4 vec4_mul(vec4 vector_0, vec4 vector_1) 
{
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = vector_0.elements[i] * vector_1.elements[i];
    }
    return result;
}

FORCEINLINE void vec4_mul_by(vec4* vector_0, vec4 vector_1) 
{
    for (u64 i = 0; i < 4; ++i) {
        vector_0->elements[i] *= vector_1.elements[i];
    }
}

FORCEINLINE vec4 vec4_div(vec4 vector_0, vec4 vector_1) 
{
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = vector_0.elements[i] / vector_1.elements[i];
    }
    return result;
}

FORCEINLINE void vec4_div_by(vec4* vector_0, vec4 vector_1) 
{
    for (u64 i = 0; i < 4; ++i) {
        vector_0->elements[i] /= vector_1.elements[i];
    }
}

FORCEINLINE f32 vec4_length_squared(vec4 vector) 
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;
}

FORCEINLINE f32 vec4_length(vec4 vector) 
{
    return gsqrt(vec4_length_squared(vector));
}

FORCEINLINE void vec4_normalize(vec4* vector) 
{
    const f32 length = vec4_length(*vector);
    vector->x /= length;
    vector->y /= length;
    vector->z /= length;
    vector->w /= length;
}

FORCEINLINE vec4 vec4_normalized(vec4 vector) 
{
    vec4_normalize(&vector);
    return vector;
}

FORCEINLINE f32 vec4_dot(f32 a0, f32 a1, f32 a2, f32 a3, f32 b0, f32 b1, f32 b2, f32 b3) 
{
    return a0 * b0 + a1 * b1 + a2 * b2 + a3 * b3;
}

/**
 * @brief Creates and returns an identity matrix:
 * 
 * {
 *   {1, 0, 0, 0},
 *   {0, 1, 0, 0},
 *   {0, 0, 1, 0},
 *   {0, 0, 0, 1}
 * }
 * 
 * @return A new identity matrix 
 */
FORCEINLINE mat4 mat4_identity()
{
    mat4 out_matrix;
    GDF_MemZero(out_matrix.data, sizeof(f32) * 16);
    out_matrix.data[0] = 1.0f;
    out_matrix.data[5] = 1.0f;
    out_matrix.data[10] = 1.0f;
    out_matrix.data[15] = 1.0f;
    return out_matrix;
}

/**
 * @brief Returns the result of multiplying matrix_0 and matrix_1.
 * 
 * @param matrix_0 The first matrix to be multiplied.
 * @param matrix_1 The second matrix to be multiplied.
 * @return The result of the matrix multiplication.
 */
FORCEINLINE mat4 mat4_mul(mat4 matrix_0, mat4 matrix_1)
{
    mat4 out_matrix = mat4_identity();

    const f32* m1_ptr = matrix_0.data;
    const f32* m2_ptr = matrix_1.data;
    f32* dst_ptr = out_matrix.data;

    for (i32 i = 0; i < 4; i++)
    {
        for (i32 j = 0; j < 4; j++)
        {
            *dst_ptr =
                m1_ptr[0] * m2_ptr[0 + j] +
                m1_ptr[1] * m2_ptr[4 + j] +
                m1_ptr[2] * m2_ptr[8 + j] +
                m1_ptr[3] * m2_ptr[12 + j];
            dst_ptr++;
        }
        m1_ptr += 4;
    }
    return out_matrix;
}

/**
 * @brief Creates and returns an orthographic projection matrix. Typically used to
 * render flat or 2D scenes.
 * 
 * @param left The left side of the view frustum.
 * @param right The right side of the view frustum.
 * @param bottom The bottom side of the view frustum.
 * @param top The top side of the view frustum.
 * @param near_clip The near clipping plane distance.
 * @param far_clip The far clipping plane distance.
 * @return A new orthographic projection matrix. 
 */
FORCEINLINE mat4 mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_clip, f32 far_clip)
{
    mat4 out_matrix = mat4_identity();

    f32 lr = 1.0f / (left - right);
    f32 bt = 1.0f / (bottom - top);
    f32 nf = 1.0f / (near_clip - far_clip);

    out_matrix.data[0] = -2.0f * lr;
    out_matrix.data[5] = -2.0f * bt;
    out_matrix.data[10] = 2.0f * nf;

    out_matrix.data[12] = (left + right) * lr;
    out_matrix.data[13] = (top + bottom) * bt;
    out_matrix.data[14] = (far_clip + near_clip) * nf;
    return out_matrix;
}

/**
 * @brief Creates and returns a perspective matrix. Typically used to render 3d scenes.
 * 
 * @param fov_radians The field of view in radians.
 * @param aspect_ratio The aspect ratio.
 * @param near_clip The near clipping plane distance.
 * @param far_clip The far clipping plane distance.
 * @return A new perspective matrix. 
 */
FORCEINLINE mat4 mat4_perspective(f32 fov_radians, f32 aspect_ratio, f32 near_clip, f32 far_clip)
{
    f32 half_tan_fov = gtan(fov_radians * 0.5f);
    mat4 out_matrix;
    GDF_MemZero(out_matrix.data, sizeof(f32) * 16);
    out_matrix.data[0] = 1.0f / (aspect_ratio * half_tan_fov);
    out_matrix.data[5] = 1.0f / half_tan_fov;
    out_matrix.data[10] = -((far_clip + near_clip) / (far_clip - near_clip));
    out_matrix.data[11] = -1.0f;
    out_matrix.data[14] = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));
    return out_matrix;
}

/**
 * @brief Creates and returns a look-at matrix, or a matrix looking 
 * at target from the perspective of position.
 * 
 * @param position The position of the matrix.
 * @param target The position to "look at".
 * @param up The up vector.
 * @return A matrix looking at target from the perspective of position. 
 */
FORCEINLINE mat4 mat4_look_at(vec3 position, vec3 target, vec3 up)
{
    mat4 out_matrix;
    vec3 z_axis;
    z_axis.x = target.x - position.x;
    z_axis.y = target.y - position.y;
    z_axis.z = target.z - position.z;

    z_axis = vec3_normalized(z_axis);
    vec3 x_axis = vec3_normalized(vec3_cross(z_axis, up));
    vec3 y_axis = vec3_cross(x_axis, z_axis);

    out_matrix.data[0] = x_axis.x;
    out_matrix.data[1] = y_axis.x;
    out_matrix.data[2] = -z_axis.x;
    out_matrix.data[3] = 0;
    out_matrix.data[4] = x_axis.y;
    out_matrix.data[5] = y_axis.y;
    out_matrix.data[6] = -z_axis.y;
    out_matrix.data[7] = 0;
    out_matrix.data[8] = x_axis.z;
    out_matrix.data[9] = y_axis.z;
    out_matrix.data[10] = -z_axis.z;
    out_matrix.data[11] = 0;
    out_matrix.data[12] = -vec3_dot(x_axis, position);
    out_matrix.data[13] = -vec3_dot(y_axis, position);
    out_matrix.data[14] = vec3_dot(z_axis, position);
    out_matrix.data[15] = 1.0f;

    return out_matrix;
}

/**
 * @brief Returns a transposed copy of the provided matrix (rows->colums)
 * 
 * @param matrix The matrix to be transposed.
 * @return A transposed copy of of the provided matrix.
 */
FORCEINLINE mat4 mat4_transposed(mat4 matrix)
{
    mat4 out_matrix = mat4_identity();
    out_matrix.data[0] = matrix.data[0];
    out_matrix.data[1] = matrix.data[4];
    out_matrix.data[2] = matrix.data[8];
    out_matrix.data[3] = matrix.data[12];
    out_matrix.data[4] = matrix.data[1];
    out_matrix.data[5] = matrix.data[5];
    out_matrix.data[6] = matrix.data[9];
    out_matrix.data[7] = matrix.data[13];
    out_matrix.data[8] = matrix.data[2];
    out_matrix.data[9] = matrix.data[6];
    out_matrix.data[10] = matrix.data[10];
    out_matrix.data[11] = matrix.data[14];
    out_matrix.data[12] = matrix.data[3];
    out_matrix.data[13] = matrix.data[7];
    out_matrix.data[14] = matrix.data[11];
    out_matrix.data[15] = matrix.data[15];
    return out_matrix;
}

/**
 * @brief Creates and returns an inverse of the provided matrix.
 * 
 * @param matrix The matrix to be inverted.
 * @return A inverted copy of the provided matrix. 
 */
FORCEINLINE mat4 mat4_inverse(mat4 matrix)
{
    const f32* m = matrix.data;

    f32 t0 = m[10] * m[15];
    f32 t1 = m[14] * m[11];
    f32 t2 = m[6] * m[15];
    f32 t3 = m[14] * m[7];
    f32 t4 = m[6] * m[11];
    f32 t5 = m[10] * m[7];
    f32 t6 = m[2] * m[15];
    f32 t7 = m[14] * m[3];
    f32 t8 = m[2] * m[11];
    f32 t9 = m[10] * m[3];
    f32 t10 = m[2] * m[7];
    f32 t11 = m[6] * m[3];
    f32 t12 = m[8] * m[13];
    f32 t13 = m[12] * m[9];
    f32 t14 = m[4] * m[13];
    f32 t15 = m[12] * m[5];
    f32 t16 = m[4] * m[9];
    f32 t17 = m[8] * m[5];
    f32 t18 = m[0] * m[13];
    f32 t19 = m[12] * m[1];
    f32 t20 = m[0] * m[9];
    f32 t21 = m[8] * m[1];
    f32 t22 = m[0] * m[5];
    f32 t23 = m[4] * m[1];

    mat4 out_matrix;
    out_matrix.data[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    out_matrix.data[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    out_matrix.data[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    out_matrix.data[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);
    out_matrix.data[4] = (t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]);
    out_matrix.data[5] = (t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]);
    out_matrix.data[6] = (t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]);
    out_matrix.data[7] = (t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]);
    out_matrix.data[8] = (t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]);
    out_matrix.data[9] = (t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]);
    out_matrix.data[10] = (t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]);
    out_matrix.data[11] = (t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]);
    out_matrix.data[12] = (t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]);
    out_matrix.data[13] = (t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]);
    out_matrix.data[14] = (t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]);
    out_matrix.data[15] = (t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]);

    f32 determinant = m[0] * out_matrix.data[0] + m[4] * out_matrix.data[1] + m[8] * out_matrix.data[2] + m[12] * out_matrix.data[3];
    f32 inv_determinant = 1.0f / determinant;

    for (i32 i = 0; i < 16; i++)
    {
        out_matrix.data[i] *= inv_determinant;
    }

    return out_matrix;
}

/**
 * NOTE: this may not work as intended ...
 * @brief Returns the view matrix for the specified camera parameters.
 * 
 * @param pos The position the camera is in.
 * @param yaw The yaw of the camera.
 * @param pitch The pitch of the camera.
 * @return A view matrix.
 * 
 */
FORCEINLINE mat4 mat4_view(vec3 pos, f32 yaw, f32 pitch) 
{
    vec3 front;
    front.x = gcos(yaw) * gcos(pitch);
    front.y = gsin(pitch);
    front.z = gsin(yaw) * gcos(pitch);
    vec3_normalize(&front);

    vec3 up = { 0.0f, 1.0f, 0.0f };
    vec3 right = vec3_normalized(vec3_cross(up, front));
    up = vec3_cross(front, right);

    return mat4_look_at(pos, vec3_add(pos, front), up);
}

/**
 * @brief Returns a backward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FORCEINLINE vec3 mat4_backward(mat4 matrix)
{
    vec3 backward;
    backward.x = matrix.data[2];
    backward.y = matrix.data[6];
    backward.z = matrix.data[10];
    vec3_normalize(&backward);
    return backward;
}

/**
 * @brief Returns an upward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FORCEINLINE vec3 mat4_up(mat4 matrix)
{
    vec3 up;
    up.x = matrix.data[1];
    up.y = matrix.data[5];
    up.z = matrix.data[9];
    vec3_normalize(&up);
    return up;
}

/**
 * @brief Returns a downward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FORCEINLINE vec3 mat4_down(mat4 matrix)
{
    vec3 down;
    down.x = -matrix.data[1];
    down.y = -matrix.data[5];
    down.z = -matrix.data[9];
    vec3_normalize(&down);
    return down;
}

/**
 * @brief Returns a left vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FORCEINLINE vec3 mat4_left(mat4 matrix)
{
    vec3 left;
    left.x = -matrix.data[0];
    left.y = -matrix.data[4];
    left.z = -matrix.data[8];
    vec3_normalize(&left);
    return left;
}

/**
 * @brief Returns a right vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FORCEINLINE vec3 mat4_right(mat4 matrix)
{
    vec3 right;
    right.x = matrix.data[0];
    right.y = matrix.data[4];
    right.z = matrix.data[8];
    vec3_normalize(&right);
    return right;
}

// ------------------------------------------
// Quaternion
// ------------------------------------------

FORCEINLINE Quaternion quaternion_identity()
{
    return (Quaternion){0, 0, 0, 1.0f};
}

FORCEINLINE f32 quaternion_normal(Quaternion q)
{
    return gsqrt(
        q.x * q.x +
        q.y * q.y +
        q.z * q.z +
        q.w * q.w
    );
}

FORCEINLINE Quaternion quaternion_normalize(Quaternion q)
{
    f32 normal = quaternion_normal(q);
    return (Quaternion) {
        q.x / normal,
        q.y / normal,
        q.z / normal,
        q.w / normal
    };
}

FORCEINLINE Quaternion quaternion_conjugate(Quaternion q)
{
    return (Quaternion){
        -q.x,
        -q.y,
        -q.z,
        q.w};
}

FORCEINLINE Quaternion quaternion_inverse(Quaternion q)
{
    return quaternion_normalize(quaternion_conjugate(q));
}

FORCEINLINE Quaternion quaternion_mul(Quaternion q_0, Quaternion q_1)
{
    Quaternion out_quaternionernion;

    out_quaternionernion.x = q_0.x * q_1.w +
                             q_0.y * q_1.z -
                             q_0.z * q_1.y +
                             q_0.w * q_1.x;

    out_quaternionernion.y = -q_0.x * q_1.z +
                             q_0.y * q_1.w +
                             q_0.z * q_1.x +
                             q_0.w * q_1.y;

    out_quaternionernion.z = q_0.x * q_1.y -
                             q_0.y * q_1.x +
                             q_0.z * q_1.w +
                             q_0.w * q_1.z;

    out_quaternionernion.w = -q_0.x * q_1.x -
                             q_0.y * q_1.y -
                             q_0.z * q_1.z +
                             q_0.w * q_1.w;

    return out_quaternionernion;
}

FORCEINLINE f32 quaternion_dot(Quaternion q_0, Quaternion q_1)
{
    return q_0.x * q_1.x +
           q_0.y * q_1.y +
           q_0.z * q_1.z +
           q_0.w * q_1.w;
}

FORCEINLINE mat4 quaternion_to_mat4(Quaternion q)
{
    mat4 out_matrix = mat4_identity();

    // https://stackoverflow.com/questions/1556260/convert-quaternionernion-rotation-to-rotation-matrix

    Quaternion n = quaternion_normalize(q);

    out_matrix.data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    out_matrix.data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    out_matrix.data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

    out_matrix.data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    out_matrix.data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    out_matrix.data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

    out_matrix.data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    out_matrix.data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    out_matrix.data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return out_matrix;
}

// Calculates a rotation matrix based on the quaternionernion and the passed in center point.
FORCEINLINE mat4 quaternion_to_rotation_matrix(Quaternion q, vec3 center)
{
    mat4 out_matrix;

    f32* o = out_matrix.data;
    o[0] = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[1] = 2.0f * ((q.x * q.y) + (q.z * q.w));
    o[2] = 2.0f * ((q.x * q.z) - (q.y * q.w));
    o[3] = center.x - center.x * o[0] - center.y * o[1] - center.z * o[2];

    o[4] = 2.0f * ((q.x * q.y) - (q.z * q.w));
    o[5] = -(q.x * q.x) + (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[6] = 2.0f * ((q.y * q.z) + (q.x * q.w));
    o[7] = center.y - center.x * o[4] - center.y * o[5] - center.z * o[6];

    o[8] = 2.0f * ((q.x * q.z) + (q.y * q.w));
    o[9] = 2.0f * ((q.y * q.z) - (q.x * q.w));
    o[10] = -(q.x * q.x) - (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
    o[11] = center.z - center.x * o[8] - center.y * o[9] - center.z * o[10];

    o[12] = 0.0f;
    o[13] = 0.0f;
    o[14] = 0.0f;
    o[15] = 1.0f;

    return out_matrix;
}

FORCEINLINE Quaternion quaternion_from_axis_angle(vec3 axis, f32 angle, bool normalize)
{
    const f32 half_angle = 0.5f * angle;
    f32 s = gsin(half_angle);
    f32 c = gcos(half_angle);

    Quaternion q = (Quaternion){s * axis.x, s * axis.y, s * axis.z, c};
    if (normalize) 
    {
        return quaternion_normalize(q);
    }
    return q;
}

FORCEINLINE Quaternion quaternion_slerp(Quaternion q_0, Quaternion q_1, f32 percentage)
{
    Quaternion out_quaternionernion;
    // Source: https://en.wikipedia.org/wiki/Slerp
    // Only unit quaternionernions are valid rotations.
    // Normalize to avoid undefined behavior.
    Quaternion v0 = quaternion_normalize(q_0);
    Quaternion v1 = quaternion_normalize(q_1);

    // Compute the cosine of the angle between the two vectors.
    f32 dot = quaternion_dot(v0, v1);

    // If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by
    // reversing one quaternionernion.
    if (dot < 0.0f) 
    {
        v1.x = -v1.x;
        v1.y = -v1.y;
        v1.z = -v1.z;
        v1.w = -v1.w;
        dot = -dot;
    }

    const f32 DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) 
    {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        out_quaternionernion = (Quaternion){
            v0.x + ((v1.x - v0.x) * percentage),
            v0.y + ((v1.y - v0.y) * percentage),
            v0.z + ((v1.z - v0.z) * percentage),
            v0.w + ((v1.w - v0.w) * percentage)};

        return quaternion_normalize(out_quaternionernion);
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    f32 theta_0 = gacos(dot);           // theta_0 = angle between input vectors
    f32 theta = theta_0 * percentage;  // theta = angle between v0 and result
    f32 sin_theta = gsin(theta);        // compute this value only once
    f32 sin_theta_0 = gsin(theta_0);    // compute this value only once

    f32 s0 = gcos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    f32 s1 = sin_theta / sin_theta_0;

    return (Quaternion) {
        (v0.x * s0) + (v1.x * s1),
        (v0.y * s0) + (v1.y * s1),
        (v0.z * s0) + (v1.z * s1),
        (v0.w * s0) + (v1.w * s1)
    };
}

FORCEINLINE f32 deg_to_rad(f32 degrees)
{
    return degrees * DEG_TO_RAD;
}

FORCEINLINE f32 rad_to_deg(f32 radians)
{
    return radians * RAD_TO_DEG;
}