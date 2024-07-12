#pragma once

#include "core.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "engine/camera.h"
#include "engine/render/render_types.h"

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

    VkCommandBuffer cmd_buffer;
    VkCommandPool graphics_cmd_pool;
} vk_device;

typedef struct vk_pipeline {
    VkPipeline handle;
    VkPipelineLayout layout;
} vk_pipeline;

typedef struct vk_pipelines {
    vk_pipeline geometry;
    vk_pipeline geometry_wireframe;
    vk_pipeline lighting;
    vk_pipeline post_processing;
} vk_pipelines;

typedef struct vk_formats {
    VkFormat image_format;
    VkColorSpaceKHR image_color_space;
    VkFormat depth_format;
} vk_formats;

typedef struct vk_renderpasses {
    VkRenderPass main_pass;
} vk_renderpasses;

typedef struct vk_shader_modules {
    // Geometry pass
    VkShaderModule geometry_vert;
    VkShaderModule geometry_frag;
    
    // Lighting pass
    VkShaderModule lighting_vert;
    VkShaderModule lighting_frag;
    
    // Post-processing pass
    VkShaderModule post_process_vert;
    VkShaderModule post_process_frag;
} vk_shader_modules;

typedef struct vk_image {
    VkImage handle;
    VkImageView view;
} vk_image;

typedef struct vk_swapchain {
    VkSwapchainKHR handle;

    // GDF_LIST structure
    vk_image* images; 
    // GDF_LIST structure
    VkFramebuffer* framebuffers;
    u32 current_img_idx;
    VkExtent2D extent;
    u32 image_count;
} vk_swapchain;

typedef struct vk_uniform_buffer {
    VkBuffer handle;
    VkDeviceMemory memory;
    void* mapped_data;
} vk_uniform_buffer;

typedef struct vk_renderer_context { 
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
    vk_swapchain swapchain;

    // Depth image resources
    VkImage depth_image;
    VkDeviceMemory depth_image_memory;
    VkImageView depth_image_view;

    vk_pipelines pipelines;
    vk_formats formats;
    vk_renderpasses renderpasses;

    // Shader resources
    vk_shader_modules shaders;
    // GDF_LIST
    vk_uniform_buffer* uniform_buffers;
    // This field is modified then copied over to vk_uniform_buffer[n].mapped_Data
    UniformBuffer uniform_buffer_data;
    VkDescriptorPool descriptor_pool;
    // GDF_LIST
    VkDescriptorSet* descriptor_sets;
    // GDF_LIST
    VkDescriptorSetLayout* descriptor_set_layouts;

    // GDF_LIST of physical device info structs
    vk_physical_device* physical_device_info_list;
    vk_device device;

    u32 current_frame;
    u32 max_concurrent_frames;
    // Sync objects
    // GDF_LIST
    VkSemaphore* image_available_semaphores;
    // GDF_LIST
    VkSemaphore* render_finished_semaphores;
    // GDF_LIST
    VkFence* in_flight_fences;
    // GDF_LIST
    VkFence* images_in_flight;

    u32 framebuffer_width;
    u32 framebuffer_height;
    bool pending_resize_event;
    bool creating_swapchain;
    bool ready_for_use;


#ifndef GDF_RELEASE
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
} vk_renderer_context;