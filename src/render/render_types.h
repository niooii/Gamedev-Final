#pragma once

#include "core.h"

typedef enum GDF_RENDER_BACKEND_TYPE {
    GDF_RENDER_BACKEND_TYPE_VULKAN,
    GDF_RENDER_BACKEND_TYPE_OPENGL
} GDF_RENDER_BACKEND_TYPE;

typedef struct renderer_backend {
    u64 frame_number;

    bool (*initialize)(struct renderer_backend* backend, const char* application_name);

    void (*shutdown)(struct renderer_backend* backend);

    void (*resized)(struct renderer_backend* backend, u16 width, u16 height);

    bool (*begin_frame)(struct renderer_backend* backend, f32 delta_time);
    bool (*end_frame)(struct renderer_backend* backend, f32 delta_time);    
} renderer_backend;

typedef struct GDF_RenderPacket {
    f32 delta_time;
} GDF_RenderPacket;