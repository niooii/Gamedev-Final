#pragma once

#include "core.h"

#include <vulkan/vulkan.h>

#define VK_ASSERT(expr) \
{ \
    GDF_ASSERT_MSG(expr == VK_SUCCESS, "Vulkan function did not return VK_SUCCESS."); \
} \

typedef struct vk_context {
    VkInstance instance;
    VkAllocationCallbacks* allocator;
} vk_context;