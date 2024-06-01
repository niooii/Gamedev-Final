#pragma once

#include "core.h"
#include "vk_types.h"
#include "vk_image.h"

// if name is NULL/0, chooses the best one
bool vk_device_create(vk_context* context, const char* name);

// destroys the current device
void vk_device_destroy(vk_context* context);

void vk_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    vk_pdevice_swapchain_support* out_support_info);

void vk_device_query_queues(
    vk_context* context,
    VkPhysicalDevice physical_device,
    vk_pdevice_queues* queues);

bool vk_device_find_depth_format(vk_device* device);