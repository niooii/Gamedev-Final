#pragma once

#include "core.h"
#include "vk_types.h"

bool vk_pipelines_create_blocks(vk_renderer_context* context);
bool vk_pipelines_create_blocks_wireframe(vk_renderer_context* context);
bool vk_pipelines_create_lighting(vk_renderer_context* context);
bool vk_pipelines_create_post_processing(vk_renderer_context* context);
bool vk_pipelines_create_grid(vk_renderer_context* context);