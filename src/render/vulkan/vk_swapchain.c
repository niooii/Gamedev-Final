#include "vk_swapchain.h"

void __create(vk_context* context, u32 w, u32 h, vk_swapchain* swapchain);
void __destroy(vk_context* context, vk_swapchain* swapchain);

void vk_swapchain_create(
    vk_context* context,
    u32 w,
    u32 h,
    vk_swapchain* out_swapchain)
{
    __create(context, w, h, out_swapchain);
}

void vk_swapchain_recreate(
    vk_context* context,
    u32 w,
    u32 h,
    vk_swapchain* swapchain)
{
    __destroy(context, swapchain);
    __create(context, w, h, swapchain);
}

void vk_swapchain_destroy(
    vk_context* context,
    vk_swapchain* swapchain)
{
    __destroy(context, swapchain);
}

bool vk_swapchain_acquire_next_image_index(
    vk_context* context,
    vk_swapchain* swapchain,
    u64 timeout_ns,
    VkSemaphore image_available_semaphore,
    VkFence fence,
    u32* out_image_idx)
{
    VkResult r = vkAcquireNextImageKHR(
        context->device->logical,
        swapchain,
        timeout_ns,
        image_available_semaphore,
        fence,
        out_image_idx
    );

    if (r == VK_ERROR_OUT_OF_DATE_KHR)
    {
        vk_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
        return false; 
    } 
    else if (r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR)
    {
        LOG_ERR("Could not acquire swapchain image.");
        return false;
    }
}

void vk_swapchain_present(
    vk_context* context,
    vk_swapchain* swapchain,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete_semaphore,
    u32 present_image_index)
{
    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_complete_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain->handle;
    present_info.pImageIndices = &present_image_index;
    present_info.pResults = 0;

    VkResult result = vkQueuePresentKHR(present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Swapchain is out of date, suboptimal or a framebuffer resize has occurred. Trigger swapchain recreation.
        vk_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
    } else if (result != VK_SUCCESS) {
        LOG_FATAL("Failed to present swap chain image!");
    }
}

void __create(vk_context* context, u32 w, u32 h, vk_swapchain* swapchain)
{
    VkExtent2D swapchain_extent = {w, h};
    swapchain->max_frames_in_flight = 2;

    // update swapchain support in case anything changed
    vk_pdevice_swapchain_support* swapchain_support = &context->device->physical_info->sc_support_info;
    vk_device_query_swapchain_support(context->device->physical_info->handle, context->surface, swapchain_support);

    // use the first img format and switch if a better one is found
    swapchain->image_format = swapchain_support->formats[0];
    for (u32 i = 0; i < swapchain_support->format_count; i++) 
    {
        VkSurfaceFormatKHR format = swapchain_support->formats[i];
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
        {
            swapchain->image_format = format;
            break;
        }
    }

    // spec says its present on every device but find a better one
    // TODO! if turning off vsync make it immediate mode
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < swapchain_support->present_mode_count; ++i) 
    {
        VkPresentModeKHR mode = swapchain_support->present_modes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            present_mode = mode;
            break;
        }
    }

    if (swapchain_support->capabilities.currentExtent.width != UINT32_MAX)
    {
        swapchain_extent = swapchain_support->capabilities.currentExtent;
    }

    VkExtent2D min = swapchain_support->capabilities.minImageExtent;
    VkExtent2D max = swapchain_support->capabilities.maxImageExtent;
    swapchain_extent.width = CLAMP(swapchain_extent.width, min.width, max.width);
    swapchain_extent.height = CLAMP(swapchain_extent.height, min.height, max.height);

    u32 image_count = swapchain_support->capabilities.minImageCount + 1;
    image_count = CLAMP(image_count,
    swapchain_support->capabilities.minImageCount, 
    swapchain_support->capabilities.maxImageCount);

    VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchain_create_info.surface = context->surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = swapchain->image_format.format;
    swapchain_create_info.imageColorSpace = swapchain->image_format.colorSpace;
    swapchain_create_info.imageExtent = swapchain_extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (context->device->physical_info->queues.graphics_family_index != context->device->physical_info->queues.present_family_index) 
    {
        u32 queueFamilyIndices[] = {
            (u32)context->device->physical_info->queues.graphics_family_index,
            (u32)context->device->physical_info->queues.present_family_index
        };
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queueFamilyIndices;
    } 
    else 
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = 0;
    }

    swapchain_create_info.preTransform = swapchain_support->capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = 0;

    VK_ASSERT(
        vkCreateSwapchainKHR(
            context->device->logical,
            &swapchain_create_info,
            context->allocator,
            &swapchain->handle
        )
    );

    LOG_DEBUG("Created swapchain.");

    context->current_frame_num = 0;
    swapchain->image_count = 0;

    VK_ASSERT(
        vkGetSwapchainImagesKHR(
            context->device->logical, 
            swapchain->handle,
            &swapchain->image_count, 
            NULL
        )
    );
    swapchain->images = GDF_Malloc(sizeof(VkImage) * swapchain->image_count, GDF_MEMTAG_RENDERER);
    swapchain->views = GDF_Malloc(sizeof(VkImageView) * swapchain->image_count, GDF_MEMTAG_RENDERER);
    VK_ASSERT(
        vkGetSwapchainImagesKHR(
            context->device->logical, 
            swapchain->handle, 
            &swapchain->image_count, 
            swapchain->images
        )
    );

    for (u32 i = 0; i < swapchain->image_count; ++i) 
    {
        VkImageViewCreateInfo view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        view_info.image = swapchain->images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = swapchain->image_format.format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VK_ASSERT(
            vkCreateImageView(
                context->device->logical, 
                &view_info, 
                context->allocator, 
                &swapchain->views[i]
            )
        );
    }

    // TODO! this crashes for some unknown reason.
    // hardcoding it for now unlucky
    // if (!vk_device_find_depth_format(&context->device)) {
    //     context->device->depth_format = VK_FORMAT_UNDEFINED;
    //     LOG_FATAL("No depths format..");
    // }
    context->device->depth_format = VK_FORMAT_D32_SFLOAT;

    // Create depth image and its view.
    vk_image_create(
        context,
        VK_IMAGE_TYPE_2D,
        swapchain_extent.width,
        swapchain_extent.height,
        context->device->depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        TRUE,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &swapchain->depth_attachment);

    LOG_DEBUG("Swapchain created successfully.");
}

void __destroy(vk_context* context, vk_swapchain* swapchain)
{
    vk_image_destroy(context, &swapchain->depth_attachment);

    for (u32 i = 0; i < swapchain->image_count; ++i) {
        vkDestroyImageView(context->device->logical, swapchain->views[i], context->allocator);
    }

    GDF_Free(swapchain->images);
    GDF_Free(swapchain->views);

    vkDestroySwapchainKHR(context->device->logical, swapchain->handle, context->allocator);
}