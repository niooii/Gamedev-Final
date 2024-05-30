#include "renderer_backend.h"
#include "vulkan/vk_renderer.h"

bool renderer_backend_create(GDF_RENDER_BACKEND_TYPE type, renderer_backend* out_renderer_backend) {

    if (type == GDF_RENDER_BACKEND_TYPE_VULKAN) {
        out_renderer_backend->initialize = vk_renderer_init;
        out_renderer_backend->shutdown = vk_renderer_shutdown;
        out_renderer_backend->begin_frame = vk_renderer_begin_frame;
        out_renderer_backend->end_frame = vk_renderer_end_frame;
        out_renderer_backend->resized = vk_renderer_resize;

        return TRUE;
    }

    return FALSE;
}

void renderer_backend_destroy(renderer_backend* renderer_backend) {
    renderer_backend->initialize = 0;
    renderer_backend->shutdown = 0;
    renderer_backend->begin_frame = 0;
    renderer_backend->end_frame = 0;
    renderer_backend->resized = 0;
}