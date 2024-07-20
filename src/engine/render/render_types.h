#pragma once

#include "core.h"
#include "engine/math/math_types.h"
#include "engine/camera.h"
#include "game/game.h"

typedef enum GDF_RENDER_BACKEND_TYPE {
    GDF_RENDER_BACKEND_TYPE_VULKAN,
    GDF_RENDER_BACKEND_TYPE_OPENGL
} GDF_RENDER_BACKEND_TYPE;

typedef u16 GDF_Texture;

typedef enum GDF_TEXTURE_INDEX {
    GDF_TEXTURE_INDEX_DIRT,
    GDF_TEXTURE_INDEX_GRASS_TOP,
    GDF_TEXTURE_INDEX_GRASS_SIDE,

    GDF_TEXTURE_INDEX_MAX,
} GDF_TEXTURE_INDEX;

typedef struct renderer_backend {
    u64 frame_number;

    // Camera (and view and projection) stuff
    Camera* active_camera;
    u32 framebuffer_width;
    u32 framebuffer_height;

    // Holds the current state of the game
    GDF_Game* game;

    bool (*initialize)(struct renderer_backend* backend, const char* application_name);

    void (*destroy)(struct renderer_backend* backend);

    void (*resized)(struct renderer_backend* backend, u16 width, u16 height);

    bool (*begin_frame)(struct renderer_backend* backend, f32 delta_time);
    bool (*end_frame)(struct renderer_backend* backend, f32 delta_time);

    bool (*load_texture)(struct renderer_backend* backend, const char* image_path, GDF_Texture* out_texture);
} renderer_backend;

typedef struct GDF_RenderPacket {
    f32 delta_time;
} GDF_RenderPacket;

typedef struct Vertex3d {
    vec3 pos;
} Vertex3d;

typedef struct UniformBuffer {
    mat4 view_projection;
} UniformBuffer;