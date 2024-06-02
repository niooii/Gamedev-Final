#pragma once

#include "render/renderer_backend.h"
#include "vk_device.h"
#include "vk_swapchain.h"
#include "vk_renderpass.h"
#include "vk_cmd_buf.h"
#include "vk_framebuffer.h"
#include "core/os/window.h"

bool vk_renderer_init(renderer_backend* backend, const char* application_name);
void vk_renderer_destroy(renderer_backend* backend);

void vk_renderer_resize(renderer_backend* backend, u16 width, u16 height);

bool vk_renderer_begin_frame(renderer_backend* backend, f32 delta_time);
bool vk_renderer_end_frame(renderer_backend* backend, f32 delta_time);