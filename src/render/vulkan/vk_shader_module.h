#pragma once

#include "core.h"
#include "core/os/io.h"
#include "vk_types.h"

void vk_shader_module_create(vk_context* context, const char* resource_path, VkShaderStageFlags stage, vk_shader_module* out_module);
void vk_shader_module_destroy(vk_shader_module* module);