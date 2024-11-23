#pragma once

#include "core.h"
#include "engine/math/math_types.h"
#include "engine/camera.h"
#include "game/game.h"

typedef enum GDF_RENDER_BACKEND_TYPE {
    GDF_RENDER_BACKEND_TYPE_VULKAN,
    GDF_RENDER_BACKEND_TYPE_OPENGL
} GDF_RENDER_BACKEND_TYPE;

typedef enum GDF_RENDER_MODE {
    GDF_RENDER_MODE_FULL,
    GDF_RENDER_MODE_WIREFRAME,
} GDF_RENDER_MODE;

typedef struct Renderer {
    u64 frame_number;

    // Camera (and view and projection) stuff
    u32 framebuffer_width;
    u32 framebuffer_height;

    // Holds the current state of the game
    GDF_Game* game;
    GDF_HashMap(ChunkCoord, RenderChunk) render_chunks;
    GDF_RENDER_MODE render_mode;

    bool (*initialize)(struct Renderer* backend, const char* application_name);

    void (*destroy)(struct Renderer* backend);

    void (*resized)(struct Renderer* backend, u16 width, u16 height);

    bool (*begin_frame)(struct Renderer* backend, f32 delta_time);
    bool (*end_frame)(struct Renderer* backend, f32 delta_time);

    // bool (*load_texture)(struct Renderer* backend, const char* image_path, GDF_Texture* out_texture);
} Renderer;

typedef struct GDF_RenderPacket {
    f32 delta_time;
} GDF_RenderPacket;

typedef struct Vertex3d {
    vec3 pos;
} Vertex3d;

// Everything needed aboutt a chunk to render it
typedef struct RenderChunk {
} RenderChunk;

typedef struct ViewProjUB {
    mat4 view_projection;
} ViewProjUB;