#pragma once

#include "engine/render/renderer_backend.h"
#include "vk_utils.h"
#include "engine/core/os/window.h"
#include "engine/math/math.h"

bool vk_renderer_init(renderer_backend* backend, const char* application_name);
void vk_renderer_destroy(renderer_backend* backend);

void vk_renderer_resize(u16 width, u16 height);

bool vk_renderer_begin_frame(f32 delta_time);
bool vk_renderer_end_frame(f32 delta_time);

void vk_renderer_set_camera(Camera* camera);