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

typedef struct vk_fence {
    VkFence handle;

    bool is_signaled;
} vk_fence;

typedef struct vk_device {
    vk_physical_device* physical_info;
    VkDevice logical;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkFormat depth_format;

    VkCommandPool graphics_cmd_pool;
} vk_device;

typedef struct vk_image {
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    u32 width;
    u32 height;
} vk_image;

typedef enum vk_renderpass_state {
    READY,
    RECORDING,
    IN_RENDER_PASS,
    RECORDING_ENDED,
    SUBMITTED,
    NOT_ALLOCATED
} vk_renderpass_state;

typedef struct vk_renderpass {
    VkRenderPass handle;
    f32 x, y, w, h;
    f32 r, g, b, a;

    f32 depth;
    u32 stencil;

    vk_renderpass_state state;
} vk_renderpass;

typedef struct vk_framebuffer {
    VkFramebuffer handle;
    u32 attachment_count;
    VkImageView* attachments;
    vk_renderpass* renderpass;
} vk_framebuffer;

typedef struct vk_swapchain {
    VkSurfaceFormatKHR image_format;
    u8 max_frames_in_flight;
    VkSwapchainKHR handle;
    u32 image_count;
    VkImage* images;
    VkImageView* views;

    vk_image depth_attachment;

    // framebuffers used for on-screen rendering.
    vk_framebuffer* framebuffers;
} vk_swapchain;

typedef enum vk_cmd_buf_state {
    CMD_BUF_STATE_READY,
    CMD_BUF_STATE_RECORDING,
    CMD_BUF_STATE_IN_RENDER_PASS,
    CMD_BUF_STATE_RECORDING_ENDED,
    CMD_BUF_STATE_SUBMITTED,
    CMD_BUF_STATE_NOT_ALLOCATED
} vk_cmd_buf_state;

typedef struct vk_cmd_buf {
    VkCommandBuffer handle;

    // Command buffer state.
    vk_cmd_buf_state state;
} vk_cmd_buf;

typedef struct vk_context {
    u32 framebuffer_width;
    u32 framebuffer_height;
    bool pending_resize_event;
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    // GDF_LIST of physical device info structs
    vk_physical_device* physical_device_info_list;
    vk_device device;

    // swapchain stuff
    vk_swapchain swapchain;
    vk_renderpass main_renderpass;
    // GDF_LIST, GDF_LIST methods should be used
    vk_cmd_buf* graphics_cmd_buf_list;
    // GDF_LIST, GDF_LIST methods should be used
    VkSemaphore* image_available_semaphores;
    // GDF_LIST, GDF_LIST methods should be used
    VkSemaphore* queue_complete_semaphores;
    u32 in_flight_fence_count;
    vk_fence* in_flight_fences;

    // holds pointers to fences which exist and are owned elsewhere.
    vk_fence** images_in_flight;
    u32 img_idx;
    u32 current_frame_num;
    bool recreating_swapchain;
    


#ifndef GDF_RELEASE
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
    // function ptrs
    // i32 (*find_memory_idx)(u32 type_filter, u32 property_flags);
} vk_context;