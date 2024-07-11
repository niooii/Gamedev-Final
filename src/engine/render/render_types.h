#pragma once

#include "core.h"
#include "engine/math/math_types.h"
#include "engine/camera.h"

typedef enum GDF_RENDER_BACKEND_TYPE {
    GDF_RENDER_BACKEND_TYPE_VULKAN,
    GDF_RENDER_BACKEND_TYPE_OPENGL
} GDF_RENDER_BACKEND_TYPE;

typedef struct renderer_backend {
    u64 frame_number;

    bool (*initialize)(struct renderer_backend* backend, const char* application_name);

    void (*destroy)(struct renderer_backend* backend);

    void (*resized)(u16 width, u16 height);

    bool (*begin_frame)(f32 delta_time);
    bool (*end_frame)(f32 delta_time);    
    void (*set_camera)(Camera* camera);    
} renderer_backend;

typedef struct GDF_RenderPacket {
    f32 delta_time;
} GDF_RenderPacket;

typedef struct Vertex3d {
    vec3 pos;
} Vertex3d;

typedef struct UniformBuffer {
    mat4 view;
    mat4 proj;
} UniformBuffer;