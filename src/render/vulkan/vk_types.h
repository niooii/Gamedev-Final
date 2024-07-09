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
    VkDevice handle;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkFormat depth_format;

    VkCommandPool graphics_cmd_pool;
} vk_device;

typedef struct vk_renderer_context {
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkPipeline graphics_full_pipeline;
    VkPipeline graphics_wireframe_pipeline;
    VkFormat image_format;
    VkColorSpaceKHR image_color_space;
    // GDF_LIST of physical device info structs
    vk_physical_device* physical_device_info_list;
    vk_device device;
    u32 framebuffer_width;
    u32 framebuffer_height;
    bool pending_resize_event;
    bool creating_swapchain;
    bool ready_for_use;


#ifndef GDF_RELEASE
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
    // function ptrs
    i32 (*find_memory_idx)(u32 type_filter, u32 property_flags);
} vk_renderer_context;