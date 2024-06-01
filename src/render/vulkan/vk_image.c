#include "vk_image.h"

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
    vk_image* out_image)
{
    out_image->width = width;
    out_image->height = height;

    // Creation info.
    VkImageCreateInfo image_create_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = width;
    image_create_info.extent.height = height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 4;
    image_create_info.arrayLayers = 1;
    image_create_info.format = format;
    image_create_info.tiling = tiling;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = usage;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT; 
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; 

    VK_ASSERT(vkCreateImage(context->device->logical, &image_create_info, context->allocator, &out_image->handle));

    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(context->device->logical, out_image->handle, &memory_requirements);

    // find the memory index
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(context->device->logical, &memory_properties);

    i32 memory_type = -1;
    for (u32 i = 0; i < memory_properties.memoryTypeCount; i++) 
    {
        // check each type to see if its bit is set to 1.
        if (memory_requirements.memoryTypeBits & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & memory_flags) == memory_flags) 
        {
            memory_type = i;
        }
    }

    if (memory_type == -1) {
        LOG_ERR("Required memory type not found. Image not valid.");
    }

    // Allocate memory
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    VK_ASSERT(vkAllocateMemory(context->device->logical, &memory_allocate_info, context->allocator, &out_image->memory));

    // Bind the memory
    VK_ASSERT(vkBindImageMemory(context->device->logical, out_image->handle, out_image->memory, 0));  // TODO: configurable memory offset.

    // Create view
    if (create_view) {
        out_image->view = 0;
        vk_image_view_create(context, format, out_image, view_aspect_flags);
    }
}

void vk_image_view_create(
    vk_context* context,
    VkFormat format,
    vk_image* image,
    VkImageAspectFlags aspect_flags)
{
    VkImageViewCreateInfo view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_create_info.image = image->handle;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_create_info.format = format;
    view_create_info.subresourceRange.aspectMask = aspect_flags;

    // TODO: Make configurable
    view_create_info.subresourceRange.baseMipLevel = 0;
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.layerCount = 1;

    VK_ASSERT(vkCreateImageView(context->device->logical, &view_create_info, context->allocator, &image->view));
}

void vk_image_destroy(vk_context* context, vk_image* image)
{
    if (image->view) 
    {
        vkDestroyImageView(context->device->logical, image->view, context->allocator);
        image->view = 0;
    }
    if (image->memory) 
    {
        vkFreeMemory(context->device->logical, image->memory, context->allocator);
        image->memory = 0;
    }
    if (image->handle) 
    {
        vkDestroyImage(context->device->logical, image->handle, context->allocator);
        image->handle = 0;
    }
}