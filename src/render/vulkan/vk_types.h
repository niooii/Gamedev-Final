#pragma once

#include "core.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#define VK_ASSERT(expr) \
{ \
    if ((expr) != VK_SUCCESS) \
    { \
        LOG_FATAL("%s returned: %s, in file: %s, line: %d\n", #expr, string_VkResult((expr)), __FILE__, __LINE__) \
        debugBreak(); \
    } \
} \

typedef struct vk_pdevice_swapchain_support {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
} vk_pdevice_swapchain_support;

typedef struct vk_pdevice_queues {
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
} vk_pdevice_queues;

typedef struct vk_physical_device {
    VkPhysicalDevice handle;
    VkPhysicalDeviceProperties properties;
    vk_pdevice_swapchain_support sc_support_info;
    vk_pdevice_queues queues;
    bool usable;
} vk_physical_device;

typedef struct vk_device {
    vk_physical_device physical_device_info;
    VkDevice logical_device;
    vk_pdevice_swapchain_support swapchain_support;
    i32 graphics_queue_index;
    i32 present_queue_index;
    i32 transfer_queue_index;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
} vk_device;

typedef struct vk_context {
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    // GDF_LIST of physical device info structs
    vk_physical_device* physical_device_info_list;
    vk_device device;
#ifndef GDF_RELEASE
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
} vk_context;