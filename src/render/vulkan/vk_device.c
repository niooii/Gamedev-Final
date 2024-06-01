#include "vk_device.h"
#include "core/containers/list.h"

bool vk_device_create(vk_context* context, const char* name)
{
    // TODO! RN CHOOSES THE FIRST ONE UNLUCKY
    // find first usable device

    vk_physical_device* phys_device_info = NULL;
    u32 device_num = GDF_LIST_GetLength(context->physical_device_info_list);
    for (u32 i = 0; i < device_num; i++)
    {
        if (context->physical_device_info_list[i].usable)
        {
            phys_device_info = &context->physical_device_info_list[i];
        }
    }

    if (phys_device_info == NULL)
    {
        LOG_FATAL("Could not find a device to use.");
        return false;
    }

    bool present_shares_graphics_queue = phys_device_info->queues.graphics_family_index == phys_device_info->queues.present_family_index;
    bool transfer_shares_graphics_queue = phys_device_info->queues.graphics_family_index == phys_device_info->queues.transfer_family_index;
    u32 index_count = 1;
    // don't create queues for shared indices
    if (!present_shares_graphics_queue) {
        index_count++;
    }
    if (!transfer_shares_graphics_queue) {
        index_count++;
    }
    u32 indices[index_count];
    u8 index = 0;
    indices[index++] = phys_device_info->queues.graphics_family_index;
    if (!present_shares_graphics_queue) {
        indices[index++] = phys_device_info->queues.present_family_index;
    }
    if (!transfer_shares_graphics_queue) {
        indices[index++] = phys_device_info->queues.transfer_family_index;
    }

    VkDeviceQueueCreateInfo queue_create_infos[index_count];
    for (u32 i = 0; i < index_count; ++i) {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = indices[i];
        queue_create_infos[i].queueCount = 1;
        if (indices[i] == phys_device_info->queues.graphics_family_index) {
            queue_create_infos[i].queueCount = 2;
        }
        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = 0;
        f32 queue_priority = 1.0f;
        queue_create_infos[i].pQueuePriorities = &queue_priority;
    }

    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE; 

    VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_create_info.queueCreateInfoCount = index_count;
    device_create_info.pQueueCreateInfos = queue_create_infos;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = 1;
    const char* extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    device_create_info.ppEnabledExtensionNames = &extension_names;

    context->device.physical_info = phys_device_info;

    // Create the device.
    VK_ASSERT(
        vkCreateDevice(
            phys_device_info->handle,
            &device_create_info,
            context->allocator,
            &context->device.logical
        )
    );

    LOG_DEBUG("Created logical device.");

    // Get queues.
    vkGetDeviceQueue(
        context->device.logical,
        context->device.physical_info->queues.graphics_family_index,
        0,
        &context->device.graphics_queue);

    vkGetDeviceQueue(
        context->device.logical,
        context->device.physical_info->queues.present_family_index,
        0,
        &context->device.present_queue);

    vkGetDeviceQueue(
        context->device.logical,
        context->device.physical_info->queues.transfer_family_index,
        0,
        &context->device.transfer_queue);
    
    LOG_DEBUG("Got queues.");

    return true;
}

void vk_device_destroy(vk_context* context)
{
    // TODO! destroy queues, then VkDevice, then free extra memory allocated by me.
    vkDestroyDevice(context->device.logical, context->allocator);
}

void vk_device_query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, vk_pdevice_swapchain_support* out_support_info)
{
    VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device,
        surface,
        &out_support_info->capabilities));

    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device,
        surface,
        &out_support_info->format_count,
        0));

    if (out_support_info->format_count != 0) {
        if (!out_support_info->formats) {
            out_support_info->formats = GDF_Malloc(sizeof(VkSurfaceFormatKHR) * out_support_info->format_count, GDF_MEMTAG_RENDERER);
        }
        VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device,
            surface,
            &out_support_info->format_count,
            out_support_info->formats));
    }

    // Present modes
    VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device,
        surface,
        &out_support_info->present_mode_count,
        0));
    if (out_support_info->present_mode_count != 0) {
        if (!out_support_info->present_modes) {
            out_support_info->present_modes = GDF_Malloc(sizeof(VkPresentModeKHR) * out_support_info->present_mode_count, GDF_MEMTAG_RENDERER);
        }
        VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device,
            surface,
            &out_support_info->present_mode_count,
            out_support_info->present_modes));
    }
}

void vk_device_query_queues(vk_context* context, VkPhysicalDevice physical_device, vk_pdevice_queues* queues)
{
    queues->graphics_family_index = -1;
    queues->present_family_index = -1;
    queues->compute_family_index = -1;
    queues->transfer_family_index = -1;

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, 0);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families);

    u8 min_transfer_score = 255;
    for (u32 i = 0; i < queue_family_count; ++i) {
        u8 current_transfer_score = 0;

        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queues->graphics_family_index = i;
            ++current_transfer_score;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            queues->compute_family_index = i;
            ++current_transfer_score;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            // Take the index if it is the current lowest. This increases the
            // liklihood that it is a dedicated transfer queue.
            if (current_transfer_score <= min_transfer_score) {
                min_transfer_score = current_transfer_score;
                queues->transfer_family_index = i;
            }
        }

        VkBool32 supports_present = VK_FALSE;
        VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, context->surface, &supports_present));
        if (supports_present) {
            queues->present_family_index = i;
        }
    }
}

bool vk_device_find_depth_format(vk_device* device)
{
    // we need to find one of these
    const u64 candidate_count = 3;
    VkFormat candidates[3] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    for (u64 i = 0; i < candidate_count; i++) 
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(device->physical_info->handle, candidates[i], &properties);

        if ((properties.linearTilingFeatures & flags) == flags) 
        {
            device->depth_format = candidates[i];
            return true;
        } 
        else if ((properties.optimalTilingFeatures & flags) == flags) 
        {
            device->depth_format = candidates[i];
            return true;
        }
    }

    return false;
}