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
    vk_pdevice_swapchain_support sc_support_info;
    vk_pdevice_queues queues;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
    bool usable;
} vk_physical_device;

typedef struct vk_device {
    vk_physical_device* physical_info;
    VkDevice logical;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkFormat depth_format;
} vk_device;

typedef struct vk_image {
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    u32 width;
    u32 height;
} vk_image;

typedef struct vk_swapchain {
    VkSurfaceFormatKHR image_format;
    u8 max_frames_in_flight;
    VkSwapchainKHR handle;
    u32 image_count;
    VkImage* images;
    VkImageView* views;

    vk_image depth_attachment;
} vk_swapchain;

typedef struct vk_context {
    u32 framebuffer_width;
    u32 framebuffer_height;
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    // GDF_LIST of physical device info structs
    vk_physical_device* physical_device_info_list;
    vk_device device;

    // swapchain stuff
    vk_swapchain swapchain;
    u32 img_idx;
    u32 current_frame_num;
    bool recreating_swapchain;
#ifndef GDF_RELEASE
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
} vk_context;