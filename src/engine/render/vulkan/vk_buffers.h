#pragma once

#include "core.h"
#include "vk_utils.h"
#include "vk_types.h"
#include "engine/render/render_types.h"

bool vk_buffers_create(
    vk_device* device,
    u32 alloc_size,
    u32 usage_flags,
    u32 mem_property_flags,
    vk_buffer* out_buf
);

bool vk_buffers_create_vertex(
    vk_device* device,
    Vertex3d* vertices,
    u32 vertex_count,
    vk_buffer* out_buf
);

bool vk_buffers_create_index(
    vk_device* device,
    u16* indices,
    u32 index_count,
    vk_buffer* out_buf
);

void vk_buffers_destroy(
    vk_device* device,
    vk_buffer* buf
);

bool vk_buffers_create_uniform(
    vk_device* device,
    u32 size,
    vk_uniform_buffer* out_uniform_buf
);

void vk_buffers_destroy_uniform(
    vk_device* device,
    vk_uniform_buffer* uniform_buf
);