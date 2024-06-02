#pragma once

#include "core.h"

#include "vk_types.h"

void vulkan_framebuffer_create(
    vk_context* context,
    vk_renderpass* renderpass,
    u32 width,
    u32 height,
    u32 attachment_count,
    VkImageView* attachments,
    vk_framebuffer* out_framebuffer
);

void vk_framebuffer_destroy(vk_context* context, vk_framebuffer* framebuffer);