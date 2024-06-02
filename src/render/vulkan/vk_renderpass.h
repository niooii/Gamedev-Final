#pragma once

#include "core.h"

#include "vk_types.h"

void vk_renderpass_create(
    vk_context* context, 
    vk_renderpass* out_renderpass,
    f32 x, f32 y, f32 w, f32 h,
    f32 r, f32 g, f32 b, f32 a,
    f32 depth,
    u32 stencil);

void vk_renderpass_destroy(vk_context* context, vk_renderpass* renderpass);

void vk_renderpass_begin(
    vk_cmd_buf* cmd_buf, 
    vk_renderpass* renderpass,
    VkFramebuffer frame_buffer);

void vk_renderpass_end(vk_cmd_buf* cmd_buf, vk_renderpass* renderpass);