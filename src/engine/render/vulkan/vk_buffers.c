#include "vk_buffers.h"

bool vk_buffers_create(
    vk_renderer_context* context,
    u32 alloc_size,
    u32 usage_flags,
    u32 mem_property_flags,
    vk_buffer* out_buf
)
{
    vk_device* device = &context->device;
    VkBufferCreateInfo buf_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = alloc_size,
        .usage = usage_flags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    
    VK_RETURN_FALSE_ASSERT(
        vkCreateBuffer(device->handle, &buf_info, NULL, &out_buf->handle)
    );

    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(device->handle, out_buf->handle, &mem_req);

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_req.size,
        .memoryTypeIndex = vk_utils_find_memory_type_idx(context, mem_req.memoryTypeBits, mem_property_flags)
    };
    VK_RETURN_FALSE_ASSERT(
        vkAllocateMemory(device->handle, &alloc_info, context->device.allocator, &out_buf->memory)
    );
    VK_RETURN_FALSE_ASSERT(
        vkBindBufferMemory(device->handle, out_buf->handle, out_buf->memory, 0)
    );

    return true;
}

bool vk_buffers_create_vertex(
    vk_renderer_context* context,
    Vertex3d* vertices,
    u32 vertex_count,
    vk_buffer* out_buf
)
{
    if (
        !vk_buffers_create(
            context,
            sizeof(Vertex3d) * vertex_count,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            out_buf
        )
    )
    {
        return false;
    }

    {
        void* data;
        VK_RETURN_FALSE_ASSERT(
            vkMapMemory(context->device.handle, out_buf->memory, 0, vertex_count, 0, &data)
        );
        GDF_MemCopy(data, vertices, vertex_count);
    }
    vkUnmapMemory(context->device.handle, out_buf->memory);
    return true;
}

bool vk_buffers_create_index(
    vk_renderer_context* context,
    u16* indices,
    u32 index_count,
    vk_buffer* out_buf
)
{
    if (
        !vk_buffers_create(
            context,
            sizeof(u16) * index_count,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            out_buf
        )
    )
    {
        return false;
    }

    {
        void* data;
        VK_RETURN_FALSE_ASSERT(
            vkMapMemory(context->device.handle, out_buf->memory, 0, index_count, 0, &data)
        );
        GDF_MemCopy(data, indices, index_count);
    }
    vkUnmapMemory(context->device.handle, out_buf->memory);
    return true;
}

void vk_buffers_destroy(
    vk_renderer_context* context,
    vk_buffer* buf
)
{
    vkFreeMemory(
        context->device.handle,
        buf->memory,
        context->device.allocator
    );
    vkDestroyBuffer(
        context->device.handle,
        buf->handle,
        context->device.allocator
    );
}

bool vk_buffers_create_uniform(
    vk_renderer_context* context,
    u32 size,
    vk_uniform_buffer* out_uniform_buf
)
{
    if (
        !vk_buffers_create(
            context,
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &out_uniform_buf->buffer
        )
    )
    {
        return false;
    }

    VK_RETURN_FALSE_ASSERT(
        vkMapMemory(context->device.handle, out_uniform_buf->buffer.memory, 0, size, 0, &out_uniform_buf->mapped_data)
    );
    // TODO!
    // dont unmap memory.. for now
    return true;
}

void vk_buffers_destroy_uniform(
    vk_renderer_context* context,
    vk_uniform_buffer* uniform_buf
)
{
    vkUnmapMemory(
        context->device.handle,
        uniform_buf->buffer.memory
    );
    vk_buffers_destroy(context, &uniform_buf->buffer);
}