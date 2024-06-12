#pragma once

#include "core.h"
#include "core/os/io.h"
#include "vk_types.h"

void vk_shader_create(vk_context* context, const char* resource_path, VkShaderStageFlags stage, vk_shader* out_module);
void vk_shader_destroy(vk_shader* module);