#pragma once

#include "core.h"
#include "core/os/io.h"
#include "vk_types.h"

bool vk_shader_create(vk_context* context, const char* resource_path, vk_shader* out_shader);
void vk_shader_destroy(vk_context* context, vk_shader* shader);
void vk_shader_use(vk_context* context, vk_shader* shader);