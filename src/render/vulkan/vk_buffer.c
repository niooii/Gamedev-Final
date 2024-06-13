#include "vk_buffer.h"
#include "vk_cmd_buf.h"

bool vk_buffer_create(
    vk_context* context,
    u64 size,
    VkBufferUsageFlagBits usage,
    u32 memory_property_flags,
    bool bind_on_create,
    vk_buffer* out_buffer
)
{
    GDF_MemZero(out_buffer, sizeof(vk_buffer));
    out_buffer->total_size = size;
    out_buffer->usage = usage;
    out_buffer->memory_property_flags = memory_property_flags;

    VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // NOTE: Only used in one queue.

    VK_ASSERT(vkCreateBuffer(context->device.logical, &buffer_info, context->allocator, &out_buffer->handle));

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context->device.logical, out_buffer->handle, &requirements);
    out_buffer->memory_index = context->find_memory_idx(requirements.memoryTypeBits, out_buffer->memory_property_flags);
    if (out_buffer->memory_index == -1) {
        LOG_ERR("COULD NOT CREATE VULKAN BUFFER, MEMORY TYPE NOT FOUND");
        return false;
    }

    // memory alloc stuff
    VkMemoryAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate_info.allocationSize = requirements.size;
    allocate_info.memoryTypeIndex = (u32)out_buffer->memory_index;

    VkResult result = vkAllocateMemory(
        context->device.logical,
        &allocate_info,
        context->allocator,
        &out_buffer->memory);

    if (result != VK_SUCCESS) 
    {
        LOG_ERR("COULD NOT ALLOCATE MEMORY FOR VULKAN BUFFER.");
        return false;
    }

    if (bind_on_create) 
    {
        vk_buffer_bind(context, out_buffer, 0);
    }

    return true;
}

void vk_buffer_destroy(vk_context* context, vk_buffer* buffer)
{
    if (buffer->memory) {
        vkFreeMemory(context->device.logical, buffer->memory, context->allocator);
        buffer->memory = 0;
    }
    if (buffer->handle) {
        vkDestroyBuffer(context->device.logical, buffer->handle, context->allocator);
        buffer->handle = 0;
    }
    buffer->total_size = 0;
    buffer->usage = 0;
    buffer->is_locked = false;
}

bool vk_buffer_resize(
    vk_context* context,
    u64 new_size,
    vk_buffer* buffer,
    VkQueue queue,
    VkCommandPool pool)
{
    // basically create a new buffer and copy over data
    VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.size = new_size;
    buffer_info.usage = buffer->usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // NOTE: Only used in one queue.

    VkBuffer new_buffer;
    VK_ASSERT(vkCreateBuffer(context->device.logical, &buffer_info, context->allocator, &new_buffer));

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context->device.logical, new_buffer, &requirements);

    VkMemoryAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate_info.allocationSize = requirements.size;
    allocate_info.memoryTypeIndex = (u32)buffer->memory_index;

    VkDeviceMemory new_memory;
    VkResult result = vkAllocateMemory(context->device.logical, &allocate_info, context->allocator, &new_memory);
    if (result != VK_SUCCESS) 
    {
        LOG_ERR("COULD NOT RESIZE VK BUFFER");
        return false;
    }

    // bind new memory
    VK_ASSERT(
        vkBindBufferMemory(context->device.logical, new_buffer, new_memory, 0)
    );

    vk_buffer_copy_to(context, pool, 0, queue, buffer->handle, 0, new_buffer, 0, buffer->total_size);

    // just in case
    vkDeviceWaitIdle(context->device.logical);

    // destroy old
    if (buffer->memory) {
        vkFreeMemory(context->device.logical, buffer->memory, context->allocator);
        buffer->memory = 0;
    }
    if (buffer->handle) {
        vkDestroyBuffer(context->device.logical, buffer->handle, context->allocator);
        buffer->handle = 0;
    }

    // new properties
    buffer->total_size = new_size;
    buffer->memory = new_memory;
    buffer->handle = new_buffer;

    return true;
}

void vk_buffer_bind(vk_context* context, vk_buffer* buffer, u64 offset)
{
    VK_ASSERT(vkBindBufferMemory(context->device.logical, buffer->handle, buffer->memory, offset));
}

// call map memory instead it just makes sm more sense
void* vk_buffer_lock_memory(vk_context* context, vk_buffer* buffer, u64 offset, u64 size, u32 flags)
{
    void* data;
    VK_ASSERT(vkMapMemory(context->device.logical, buffer->memory, offset, size, flags, &data));
    return data;
}
void vk_buffer_unlock_memory(vk_context* context, vk_buffer* buffer)
{
    vkUnmapMemory(context->device.logical, buffer->memory);
}

void vk_buffer_load_data(vk_context* context, vk_buffer* buffer, u64 offset, u64 size, u32 flags, const void* data)
{
    void* data_ptr;
    VK_ASSERT(vkMapMemory(context->device.logical, buffer->memory, offset, size, flags, &data_ptr));
    GDF_MemCopy(data_ptr, data, size);
    vkUnmapMemory(context->device.logical, buffer->memory);
}

void vk_buffer_copy_to(
    vk_context* context,
    VkCommandPool pool,
    VkFence fence,
    VkQueue queue,
    VkBuffer source,
    u64 source_offset,
    VkBuffer dest,
    u64 dest_offset,
    u64 size)
{
    vkQueueWaitIdle(queue);
    vk_cmd_buf temp_command_buffer;
    vk_cmd_buf_allocate_and_begin_single_use(context, pool, &temp_command_buffer);

    VkBufferCopy copy_region;
    copy_region.srcOffset = source_offset;
    copy_region.dstOffset = dest_offset;
    copy_region.size = size;

    vkCmdCopyBuffer(temp_command_buffer.handle, source, dest, 1, &copy_region);

    vk_cmd_buf_end_single_use(context, pool, &temp_command_buffer, queue);
}