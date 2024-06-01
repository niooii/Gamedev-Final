#pragma once

#include "core.h"
#include "vk_types.h"

void vk_image_create(
    vk_context* context,
    VkImageType image_type,
    u32 width,
    u32 height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memory_flags,
    bool create_view,
    VkImageAspectFlags view_aspect_flags,
    vk_image* out_image);

void vk_image_view_create(
    vk_context* context,
    VkFormat format,
    vk_image* image,
    VkImageAspectFlags aspect_flags);

void vk_image_destroy(vk_context* context, vk_image* image);