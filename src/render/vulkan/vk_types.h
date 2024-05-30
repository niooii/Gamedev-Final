#pragma once

#include "core.h"

#include <vulkan/vulkan.h>

typedef struct vk_context {
    VkInstance instance;
    VkAllocationCallbacks* allocator;
} vk_context;