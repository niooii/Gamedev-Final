#pragma once

#include "vk_types.h"

bool vk_game_renderer_init(vk_renderer_context* context, Renderer* backend);
bool vk_game_renderer_draw(vk_renderer_context* context, Renderer* backend, u8 resource_idx, f32 dt);