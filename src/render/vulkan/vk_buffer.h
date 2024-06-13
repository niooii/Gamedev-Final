#pragma once

#include "core.h"
#include "vk_types.h"

bool vk_buffer_create(
    vk_context* context,
    u64 size,
    VkBufferUsageFlagBits usage,
    u32 memory_property_flags,
    bool bind_on_create,
    vk_buffer* out_buffer
);

void vk_buffer_destroy(vk_context* context, vk_buffer* buffer);

bool vk_buffer_resize(
    vk_context* context,
    u64 new_size,
    vk_buffer* buffer,
    VkQueue queue,
    VkCommandPool pool);

void vk_buffer_bind(vk_context* context, vk_buffer* buffer, u64 offset);

void* vk_buffer_lock_memory(vk_context* context, vk_buffer* buffer, u64 offset, u64 size, u32 flags);
void vk_buffer_unlock_memory(vk_context* context, vk_buffer* buffer);

void vk_buffer_load_data(vk_context* context, vk_buffer* buffer, u64 offset, u64 size, u32 flags, const void* data);

void vk_buffer_copy_to(
    vk_context* context,
    VkCommandPool pool,
    VkFence fence,
    VkQueue queue,
    VkBuffer source,
    u64 source_offset,
    VkBuffer dest,
    u64 dest_offset,
    u64 size);