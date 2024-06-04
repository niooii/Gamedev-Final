#pragma once

#include "def.h"
#include "math_types.h"

#define PI 3.14159265358979323846f
#define PI_2 2.0f * K_PI
#define HALF_PI 0.5f * K_PI
#define QUARTER_PI 0.25f * K_PI
#define ONE_OVER_PI 1.0f / K_PI
#define ONE_OVER_TWO_PI 1.0f / K_PI_2
#define SQRT_TWO 1.41421356237309504880f
#define SQRT_THREE 1.73205080756887729352f
#define SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define DEG_TO_RAD K_PI / 180.0f
#define RAD_TO_DEG 180.0f / K_PI

// The multiplier to convert seconds to milliseconds.
#define SEC_TO_MS 1000.0f

// The multiplier to convert milliseconds to seconds.
#define MS_TO_SEC 0.001f

// A huge number that should be larger than any valid number used.
#define INFINITY 1e30f

// Smallest positive number where 1.0 + FLOAT_EPSILON != 0
#define FLOAT_EPSILON 1.192092896e-07f

f32 sin(f32 x);
f32 cos(f32 x);
f32 acos(f32 x);
f32 tan(f32 x);
f32 sqrt(f32 x);
f32 abs(f32 x);

i32 random();
i32 random_in_range();

f32 frandom();
f32 frandom_in_range();

// ------------------------------------------
// Vector 2
// ------------------------------------------

FORCEINLINE vec2 vec2_create(f32 x, f32 y) 
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
    return sqrt(vec2_length_squared(vector));
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
    if (abs(vector_0.x - vector_1.x) > tolerance) {
        return false;
    }
    if (abs(vector_0.y - vector_1.y) > tolerance) {
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

FORCEINLINE vec3 vec3_create(f32 x, f32 y, f32 z) 
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
    return sqrt(vec3_length_squared(vector));
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
    if (abs(vector_0.x - vector_1.x) > tolerance) {
        return false;
    }
    if (abs(vector_0.y - vector_1.y) > tolerance) {
        return false;
    }
    if (abs(vector_0.z - vector_1.z) > tolerance) {
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

FORCEINLINE vec4 vec4_create(f32 x, f32 y, f32 z, f32 w) 
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
    return sqrt(vec4_length_squared(vector));
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