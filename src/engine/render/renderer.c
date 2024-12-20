#include "renderer.h"
#include "renderer_backend.h"
#include <stdio.h>
#include "core.h"
#include "core/os/window.h"

// Backend render context.
static Renderer* backend = NULL;

bool GDF_InitRenderer(GDF_RENDER_BACKEND_TYPE render_backend_type) 
{
    backend = GDF_Malloc(sizeof(*backend), GDF_MEMTAG_RENDERER);

    renderer_backend_create(render_backend_type, backend);
    backend->frame_number = 0;
    GDF_GetWindowSize(&backend->framebuffer_width, &backend->framebuffer_height);

    if (!backend->initialize(backend, "TODO! temp app name")) {
        return false;
    }

    return true;
}

Renderer* renderer_get_instance()
{
    return backend;
}

void GDF_DestroyRenderer() 
{
    backend->destroy(backend);
    GDF_Free(backend);
}

bool renderer_begin_frame(f32 delta_time) 
{
    return backend->begin_frame(backend, delta_time);
}

bool renderer_end_frame(f32 delta_time) 
{
    bool result = backend->end_frame(backend, delta_time);
    backend->frame_number++;
    return result;
}

void renderer_resize(u16 width, u16 height)
{
    backend->framebuffer_width = width;
    backend->framebuffer_height = height;
    backend->resized(backend, width, height);
}

void renderer_set_camera(GDF_Camera* camera)
{
    backend->game->main_camera = camera;
}

bool renderer_draw_frame(GDF_RenderPacket* packet) 
{
    if (renderer_begin_frame(packet->delta_time)) {

        bool result = renderer_end_frame(packet->delta_time);

        if (!result) {
            LOG_ERR("Failed to end frame.");
            return false;
        }
    }

    return true;
}

// bool GDF_RENDERER_LoadTexture(const char* image_path, GDF_Texture* out_texture)
// {
//     return backend->load_texture(backend, image_path, out_texture);
// }