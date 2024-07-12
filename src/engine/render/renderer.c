#include "renderer.h"
#include "renderer_backend.h"

#include "core.h"

// Backend render context.
static renderer_backend* backend = NULL;

bool GDF_InitRenderer(GDF_RENDER_BACKEND_TYPE render_backend_type) 
{
    backend = GDF_Malloc(sizeof(*backend), GDF_MEMTAG_RENDERER);

    renderer_backend_create(render_backend_type, backend);
    backend->frame_number = 0;

    if (!backend->initialize(backend, "TODO! temp app name")) {
        return false;
    }

    return true;
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

void GDF_RENDERER_Resize(u16 width, u16 height)
{
    backend->resized(backend, width, height);
}

void GDF_RENDERER_SetCamera(Camera* camera)
{
    backend->active_camera = camera;
}

bool GDF_RENDERER_DrawFrame(GDF_RenderPacket* packet) 
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
