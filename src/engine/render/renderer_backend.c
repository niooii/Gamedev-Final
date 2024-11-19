#include "renderer_backend.h"
#include "vulkan/vk_renderer.h"

bool renderer_backend_create(GDF_RENDER_BACKEND_TYPE type, Renderer* out_renderer_backend) {

    if (type == GDF_RENDER_BACKEND_TYPE_VULKAN) {
        out_renderer_backend->initialize = vk_renderer_init;
        out_renderer_backend->destroy = vk_renderer_destroy;
        out_renderer_backend->begin_frame = vk_renderer_begin_frame;
        out_renderer_backend->end_frame = vk_renderer_end_frame;
        out_renderer_backend->resized = vk_renderer_resize;
    }

    GDF_Game* game_inst = GDF_GAME_GetInstance();
    if (game_inst == NULL)
    {
        LOG_FATAL("WTF MAN");
    }
    out_renderer_backend->game = game_inst;
    GDF_ASSERT(out_renderer_backend->game != NULL);

    return true;
}

void renderer_backend_destroy(Renderer* renderer_backend) {
    renderer_backend->initialize = 0;
    renderer_backend->destroy = 0;
    renderer_backend->begin_frame = 0;
    renderer_backend->end_frame = 0;
    renderer_backend->resized = 0;
}