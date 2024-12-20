#pragma once

#include "core.h"
#include <vulkan/vulkan.h>
#include "vk_types.h"

#define MAX_TEXTURES = 2048

// This initializes the texture array buffer
// and copies block_texture_ids[] from block.c to a storage buffer.
bool vk_block_textures_init(vk_renderer_context* context, vk_block_textures* out_texture_manager);

bool vk_block_textures_destroy(vk_block_textures* texture_manager);