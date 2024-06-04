#include "vk_fence.h"

void vk_fence_create(vk_context* context, bool create_signaled, vk_fence* out_fence)
{
    out_fence->is_signaled = create_signaled;
    VkFenceCreateInfo fence_create_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    if (out_fence->is_signaled) 
    {
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VK_ASSERT(
        vkCreateFence(
            context->device.logical,
            &fence_create_info,
            context->allocator,
            &out_fence->handle
        )
    );
}

void vk_fence_destroy(vk_context* context, vk_fence* fence)
{
    if (fence->handle) {
        vkDestroyFence(
            context->device.logical,
            fence->handle,
            context->allocator
        );
        fence->handle = NULL;
    }
    fence->is_signaled = false;
}

bool vk_fence_wait(vk_context* context, vk_fence* fence, u64 timeout_ns)
{
    if (fence->is_signaled)
        return true;
    
    VkResult r = vkWaitForFences(context->device.logical, 1, &fence->handle, VK_TRUE, timeout_ns);
    switch (r)
    {
        case VK_SUCCESS:
            fence->is_signaled = true;
            return true;
        case VK_TIMEOUT:
            LOG_WARN("Timed out while waiting for fence. Waited %lu ns.", timeout_ns);
            break;
        case VK_ERROR_DEVICE_LOST:
            LOG_ERR("Lost access to device while waiting for fence.");
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            LOG_ERR("Host ran out of memory while waiting for fence.");
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            LOG_ERR("Device ran out of memory while waiting for fence.");
            break;
        default:
            LOG_ERR("Unknown error while waiting for fence.");
            break;
    }

    return false;
}

void vk_fence_reset(vk_context* context, vk_fence* fence) 
{
    if (fence->is_signaled) 
    {
        VK_ASSERT(vkResetFences(context->device.logical, 1, &fence->handle));
        fence->is_signaled = false;
    }
}