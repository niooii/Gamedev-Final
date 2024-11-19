#pragma once

#include "engine/render/renderer_backend.h"
#include "vk_utils.h"
#include "core/os/window.h"
#include "engine/math/math.h"
#include "vk_buffers.h"

bool vk_renderer_init(Renderer* backend, const char* application_name);
void vk_renderer_destroy(Renderer* backend);

void vk_renderer_resize(Renderer* backend, u16 width, u16 height);

bool vk_renderer_begin_frame(Renderer* backend, f32 delta_time);
bool vk_renderer_end_frame(Renderer* backend, f32 delta_time);
