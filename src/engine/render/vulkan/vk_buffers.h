#pragma once

#include "core.h"
#include "vk_utils.h"
#include "vk_types.h"
#include "engine/render/render_types.h"

bool vk_buffers_create(
    vk_renderer_context* context,
    u32 alloc_size,
    u32 usage_flags,
    u32 mem_property_flags,
    vk_buffer* out_buf
);

bool vk_buffers_create_vertex(
    vk_renderer_context* context,
    Vertex3d* vertices,
    u32 vertex_count,
    vk_buffer* out_buf
);

bool vk_buffers_create_index(
   vk_renderer_context* context,
    u16* indices,
    u32 index_count,
    vk_buffer* out_buf
);

void vk_buffers_destroy(
    vk_renderer_context* context,
    vk_buffer* buf
);

bool vk_buffers_create_uniform(
    vk_renderer_context* context,
    u32 size,
    vk_uniform_buffer* out_uniform_buf
);

void vk_buffers_destroy_uniform(
    vk_renderer_context* context,
    vk_uniform_buffer* uniform_buf
);

bool vk_buffers_create_single_use_command(
    vk_renderer_context* context,
    VkCommandBuffer* out_command_buf
);

void vk_buffers_destroy_single_use_command(
    vk_renderer_context* context,
    VkCommandBuffer* command_buf
);