#pragma once

#include "core.h"
#include "vk_types.h"
#include "vk_device.h"

void vk_swapchain_create(
    vk_context* context,
    u32 w,
    u32 h,
    vk_swapchain* out_swapchain);

void vk_swapchain_recreate(
    vk_context* context,
    u32 w,
    u32 h,
    vk_swapchain* swapchain);

void vk_swapchain_destroy(
    vk_context* context,
    vk_swapchain* swapchain);

bool vk_swapchain_acquire_next_image_index(
    vk_context* context,
    vk_swapchain* swapchain,
    u64 timeout_ns,
    VkSemaphore image_available_semaphore,
    VkFence fence,
    u32* out_image_idx);

void vk_swapchain_present(
    vk_context* context,
    vk_swapchain* swapchain,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete_semaphore,
    u32 present_image_index);