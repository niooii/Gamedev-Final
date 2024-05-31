#pragma once

#include "core.h"
#include "vk_types.h"

// if name is NULL/0, chooses the best one
bool vk_device_create(vk_context* context, const char* name);

// destroys the current device
void vk_device_destroy(vk_context* context);

void vk_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    vk_device_swapchain_support* out_support_info);