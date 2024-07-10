#include "vk_renderer.h"
#include "vk_types.h"
#include "core/containers/list.h"
#include "vk_os.h"

static vk_renderer_context context;
static u16 new_framebuf_w;
static u16 new_framebuf_h;

VKAPI_ATTR VkBool32 VKAPI_CALL __vk_dbg_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
    switch (message_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_TRACE(callback_data->pMessage);
            break;
    }
    return VK_FALSE;
}

void __query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, vk_pdevice_swapchain_support* out_support_info)
{
    VK_ASSERT(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            physical_device,
            surface,
            &out_support_info->capabilities
        )
    );

    VK_ASSERT(
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device,
            surface,
            &out_support_info->format_count,
            NULL
        )
    );

    // Surface formats
    if (out_support_info->format_count != 0) 
    {
        if (!out_support_info->formats) 
        {
            out_support_info->formats = GDF_Malloc(sizeof(VkSurfaceFormatKHR) * out_support_info->format_count, GDF_MEMTAG_RENDERER);
        }
        VK_ASSERT(
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                physical_device,
                surface,
                &out_support_info->format_count,
                out_support_info->formats
            )
        );
    }

    // Present modes
    VK_ASSERT(
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device,
            surface,
            &out_support_info->present_mode_count,
            0
        )
    );
    if (out_support_info->present_mode_count != 0) 
    {
        if (!out_support_info->present_modes) 
        {
            out_support_info->present_modes = GDF_Malloc(sizeof(VkPresentModeKHR) * out_support_info->present_mode_count, GDF_MEMTAG_RENDERER);
        }
        VK_ASSERT(
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                physical_device,
                surface,
                &out_support_info->present_mode_count,
                out_support_info->present_modes
            )
        );
    }
}

// TODO! initialize lighting and postprocessing pipelines
static bool __create_renderpasses_and_pipelines(vk_renderer_context* context) 
{
    /* ======================================== */
    /* ----- CREATE RENDERPASS AND SUBPASSES ----- */
    /* ======================================== */
    
    // TODO! multiple subpasses
    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS
    };

    u32 attachment_counts = 2;
    VkAttachmentDescription attachments[2];

    // color
    VkAttachmentDescription color_attachment = {
        .format = context->formats.image_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .flags = 0
    };

    attachments[0] = color_attachment;

    VkAttachmentReference color_attachment_ref = {
        // this is the index in the "attachments" array
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    // depth
    VkAttachmentDescription depth_attachment = {
        .format = context->formats.depth_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .flags = 0
    };

    attachments[1] = depth_attachment;

    VkAttachmentReference depth_attachment_ref = {
        // this is the index in the "attachments" array
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    // TODO! other attachments

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0
    };

    VkRenderPassCreateInfo rp_create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
        .attachmentCount = attachment_counts,
        .pAttachments = attachments,
    };

    VK_ASSERT(
        vkCreateRenderPass(
            context->device.handle,
            &rp_create_info,
            context->allocator,
            &context->renderpasses.main_pass
        )
    );

    // Configure graphics pipeline

    // load all (built in) shaders
    if (!vk_utils_create_shader_module(context, "resources/shaders/geometry_base.vert.spv", &context->shaders.geometry_vert)) {
        LOG_ERR("Failed to create geometry pass vertex shader. exiting...");
        return false;
    }
    if (!vk_utils_create_shader_module(context, "resources/shaders/geometry_base.frag.spv", &context->shaders.geometry_frag)) {
        LOG_ERR("Failed to create geometry pass fragment shader. exiting...");
        return false;
    }
    if (!vk_utils_create_shader_module(context, "resources/shaders/lighting_base.vert.spv", &context->shaders.lighting_vert)) {
        LOG_ERR("Failed to create lighting pass vertex shader. exiting...");
        return false;
    }
    if (!vk_utils_create_shader_module(context, "resources/shaders/lighting_base.frag.spv", &context->shaders.lighting_frag)) {
        LOG_ERR("Failed to create lighting pass fragment shader. exiting...");
        return false;
    }
    if (!vk_utils_create_shader_module(context, "resources/shaders/post_processing_base.vert.spv", &context->shaders.post_process_vert)) {
        LOG_ERR("Failed to create post processing pass vertex shader. exiting...");
        return false;
    }
    if (!vk_utils_create_shader_module(context, "resources/shaders/post_processing_base.frag.spv", &context->shaders.post_process_frag)) {
        LOG_ERR("Failed to create post processing pass fragment shader. exiting...");
        return false;
    } 

    // Create shaders for geometry pass
    VkPipelineShaderStageCreateInfo geometry_pass_vert = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = context->shaders.geometry_vert,
        .pName = "main"
    };
    VkPipelineShaderStageCreateInfo geometry_pass_frag = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = context->shaders.geometry_frag,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo geometry_pass_shaders[2] = {
        geometry_pass_vert,
        geometry_pass_frag
    };

    // Create shaders for lighting pass
    VkPipelineShaderStageCreateInfo lighting_pass_vert = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = context->shaders.lighting_vert,
        .pName = "main"
    };
    VkPipelineShaderStageCreateInfo lighting_pass_frag = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = context->shaders.lighting_frag,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo lighting_pass_shaders[2] = {
        lighting_pass_vert,
        lighting_pass_frag
    };

    // Create shaders for post processing pass
    VkPipelineShaderStageCreateInfo postprocessing_pass_vert = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = context->shaders.post_process_vert,
        .pName = "main"
    };
    VkPipelineShaderStageCreateInfo postprocessing_pass_frag = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = context->shaders.post_process_frag,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo postprocessing_pass_shaders[2] = {
        postprocessing_pass_vert,
        postprocessing_pass_frag
    };

    // Vertex input configuration
    VkVertexInputBindingDescription bindings = {
        .binding = 0,
        .stride = sizeof(Vertex3d), 
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkVertexInputAttributeDescription attributes[] = {
        {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex3d, pos)
        },
        // Add more attributes for normals, texture coordinates, etc.
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindings,
        .vertexAttributeDescriptionCount = sizeof(attributes) / sizeof(attributes[0]),
        .pVertexAttributeDescriptions = attributes
    };

    // Input assembly configuration
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .primitiveRestartEnable = VK_FALSE,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    };

    // Viewport and scissor configuration
    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) context->framebuffer_width,
        .height = (float) context->framebuffer_height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = {
            .width = context->framebuffer_width,
            .height = context->framebuffer_height
        }
    };

    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    // Rasterization configuration
    VkPipelineRasterizationStateCreateInfo rasterizer_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE
    };

    // Multisampling configuration
    VkPipelineMultisampleStateCreateInfo multisampling_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };

    // Depths stencil configuration
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE
    };

    // Color blending configuration
    VkPipelineColorBlendAttachmentState color_blend_attachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE
    };

    VkPipelineColorBlendStateCreateInfo color_blend_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment
    };

    // Pipeline layout
    VkPipelineLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    };
    
    VK_ASSERT(
        vkCreatePipelineLayout(
            context->device.handle, 
            &layout_info,
            context->allocator,
            &context->pipelines.geometry.layout
        )
    );

    // Put all configuration in graphics pipeline info struct
    VkGraphicsPipelineCreateInfo pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .renderPass = context->renderpasses.main_pass,
        .stageCount = sizeof(geometry_pass_shaders) / sizeof(VkPipelineShaderStageCreateInfo),
        .pStages = geometry_pass_shaders,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterizer_state,
        .pMultisampleState = &multisampling_state,
        .pDepthStencilState = &depth_stencil_state,
        .pColorBlendState = &color_blend_state,
        .layout = context->pipelines.geometry.layout,
        // index of geometry subpass
        .subpass = 0,
    };

    VK_ASSERT(
        vkCreateGraphicsPipelines(
            context->device.handle,
            VK_NULL_HANDLE,
            1,
            &pipeline_create_info,
            context->allocator,
            &context->pipelines.geometry.handle
        )
    );

    // Create wireframe pipeline
    // TODO! this should work bc gp_create_info has pointers to these structs, but
    // if anything happens check here
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    rasterizer_state.polygonMode = VK_POLYGON_MODE_LINE;

    VK_ASSERT(
        vkCreateGraphicsPipelines(
            context->device.handle,
            VK_NULL_HANDLE,
            1,
            &pipeline_create_info,
            context->allocator,
            &context->pipelines.geometry_wireframe.handle
        )
    );

    // repeat pipeline creation with different shaders for other pipelines
    // Lighting subpass pipeline
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    rasterizer_state.polygonMode = VK_POLYGON_MODE_FILL;
    pipeline_create_info.stageCount = sizeof(lighting_pass_shaders) / sizeof(VkPipelineShaderStageCreateInfo);
    pipeline_create_info.pStages = lighting_pass_shaders;

    VK_ASSERT(
        vkCreateGraphicsPipelines(
            context->device.handle,
            VK_NULL_HANDLE,
            1,
            &pipeline_create_info,
            context->allocator,
            &context->pipelines.lighting.handle
        )
    );

    // Post-processing subpass pipeline
    pipeline_create_info.stageCount = sizeof(postprocessing_pass_shaders) / sizeof(VkPipelineShaderStageCreateInfo);
    pipeline_create_info.pStages = postprocessing_pass_shaders;

    VK_ASSERT(
        vkCreateGraphicsPipelines(
            context->device.handle,
            VK_NULL_HANDLE,
            1,
            &pipeline_create_info,
            context->allocator,
            &context->pipelines.post_processing.handle
        )
    );

    return true;
}

// filters the context.physical_device_info list 
static void __filter_available_devices(vk_physical_device* phys_list)
{
    u32 list_len = GDF_LIST_GetLength(phys_list);
    for (u32 i = list_len; i > 0; i--)
    {
        vk_physical_device* device = &context.physical_device_info_list[i - 1]; 
        LOG_DEBUG("Checking device %s", device->properties.deviceName);

        bool device_suitable = true;
        
        // check swapchain support
        if (device->sc_support_info.format_count < 1 || device->sc_support_info.present_mode_count < 1)
        {
            device_suitable = false;
            goto filter_device_skip;
        }
        // check queues support
        if (
            device->queues.compute_family_index == -1 ||
            device->queues.graphics_family_index == -1 ||
            device->queues.present_family_index == -1 ||
            device->queues.transfer_family_index == -1
        )
        {
            LOG_DEBUG("Device does not have the required queues.");
            // LOG_DEBUG("yes %d", );
            device_suitable = false;
            goto filter_device_skip;
        }
        // TODO! Check if the device supports
        // all the required extensions

        filter_device_skip:
        if (!device_suitable)
        {
            LOG_DEBUG("Marked device \'%s\' as unusable.", device->properties.deviceName);
            LOG_DEBUG("%d", device->queues.graphics_family_index);
            LOG_DEBUG("%d", device->queues.compute_family_index);
            LOG_DEBUG("%d", device->queues.present_family_index);
            LOG_DEBUG("%d", device->queues.transfer_family_index);
            device->usable = false;
        }
    }
}

static void __create_swapchain(vk_renderer_context* context) 
{

}

// bool __recreate_swapchain(vk_renderer_context* context) 
// {
//     if (context->creating_swapchain) {
//         LOG_WARN("Already recreating swapchain calm down there");
//         return false;
//     }

//     // if the window is 0 w or h dont do anything
//     if (cached_framebuf_w == 0 || cached_framebuf_h == 0) 
//     {
//         return false;
//     }

//     context->creating_swapchain = true;

//     // in case theres any operations going on rn
//     vkDeviceWaitIdle(context->device.handle);

//     // requery details 
//     vk_device_query_swapchain_support(
//         context->device.physical_info->handle,
//         context->surface,
//         &context->device.physical_info->sc_support_info
//     );
//     vk_device_find_depth_format(&context.device);

//     vk_swapchain_recreate(
//         &context,
//         cached_framebuf_w,
//         cached_framebuf_h,
//         &context.swapchain
//     );

//     // update the sizes with cached values
//     context.framebuffer_width = cached_framebuf_w;
//     context.framebuffer_height = cached_framebuf_h;
//     context.main_renderpass.w = context.framebuffer_width;
//     context.main_renderpass.h = context.framebuffer_height;
//     cached_framebuf_w = 0;
//     cached_framebuf_h = 0;

//     // cleanup swapchain
//     for (u32 i = 0; i < context.swapchain.image_count; i++) 
//     {
//         vk_cmd_buf_free(&context, context.device.graphics_cmd_pool, &context.graphics_cmd_buf_list[i]);
//     }

//     // framebuffers
//     for (u32 i = 0; i < context.swapchain.image_count; i++) 
//     {
//         vk_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
//     }

//     context.main_renderpass.x = 0;
//     context.main_renderpass.y = 0;
//     context.main_renderpass.w = context.framebuffer_width;
//     context.main_renderpass.h = context.framebuffer_height;

//     __regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);

//     __create_cmd_bufs(backend);

//     // done recreating
//     context.recreating_swapchain = false;

//     return true;
// }

// bool __create_buffers(vk_renderer_context* context) {
//     // TODO! remove VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT and use staging buffers
//     VkMemoryPropertyFlagBits memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

//     const u64 vertex_buffer_size = sizeof(Vertex3d) * 1024 * 1024;
//     if (!vk_buffer_create(
//             context,
//             vertex_buffer_size,
//             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
//             memory_property_flags,
//             true,
//             &context->object_vertex_buffer)) {
//         LOG_ERR("Error creating vertex buffer.");
//         return false;
//     }
//     context->vertex_offset = 0;

//     const u64 index_buffer_size = sizeof(u32) * 1024 * 1024;
//     if (!vk_buffer_create(
//             context,
//             index_buffer_size,
//             VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
//             memory_property_flags,
//             true,
//             &context->object_idx_buffer)) {
//         LOG_ERR("Error creating vertex buffer.");
//         return false;
//     }
//     context->idx_offset = 0;

//     return true;
// }

void __get_queue_indices(vk_renderer_context* context, VkPhysicalDevice physical_device, vk_pdevice_queues* queues)
{
    queues->graphics_family_index = -1;
    queues->present_family_index = -1;
    queues->compute_family_index = -1;
    queues->transfer_family_index = -1;

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, 0);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families);

    for (u32 i = 0; i < queue_family_count; i++) {

        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
        {
            queues->graphics_family_index = i;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) 
        {
            queues->compute_family_index = i;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) 
        {
            queues->transfer_family_index = i;
        }

        VkBool32 supports_present = VK_FALSE;
        VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, context->surface, &supports_present));
        if (supports_present) {
            queues->present_family_index = i;
        }
    }

    LOG_DEBUG("%d", queues->graphics_family_index);
    LOG_DEBUG("%d", queues->compute_family_index);
    LOG_DEBUG("%d", queues->present_family_index);
    LOG_DEBUG("%d", queues->transfer_family_index);
}

i32 __find_memory_index(u32 type_filter, u32 property_flags) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(context.device.physical_info->handle, &memory_properties);

    for (u32 i = 0; i < memory_properties.memoryTypeCount; i++) 
    {
        // check each memory type to see if its bit is set to 1.
        if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) 
        {
            return i;
        }
    }

    LOG_WARN("Unable to find suitable memory type!");
    return -1;
}

// ===== FORWARD DECLARATIONS END =====
bool vk_renderer_init(renderer_backend* backend, const char* application_name) 
{
    // TODO! custom allocator.
    context.allocator = 0;
    context.find_memory_idx = __find_memory_index;
    
    GDF_GetWindowSize(&context.framebuffer_width, &context.framebuffer_height);
    LOG_INFO("Framebuffer size %d, %d", context.framebuffer_width, context.framebuffer_height);


    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "GDF";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; 
    create_info.pApplicationInfo = &app_info;

    // extensions
    const char** extensions = GDF_LIST_Create(const char*);
    GDF_LIST_Push(extensions, &VK_KHR_SURFACE_EXTENSION_NAME);  // Generic surface extension
    GDF_VK_GetRequiredExtensionNames(&extensions);

#ifndef GDF_RELEASE
    GDF_LIST_Push(extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    create_info.enabledExtensionCount = GDF_LIST_GetLength(extensions);
    create_info.ppEnabledExtensionNames = extensions;

    // validation layers
#ifndef GDF_RELEASE
    const char** validation_layers = GDF_LIST_Create(const char*);
    GDF_LIST_Push(validation_layers, &"VK_LAYER_KHRONOS_validation");
    create_info.enabledLayerCount = GDF_LIST_GetLength(validation_layers);
    create_info.ppEnabledLayerNames = validation_layers;
#endif
    VK_ASSERT(vkCreateInstance(&create_info, context.allocator, &context.instance));

    LOG_DEBUG("Vulkan instance initialized successfully.");

#ifndef GDF_RELEASE
    LOG_DEBUG("Creating Vulkan debugger...");
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;  //|
                                                                      //    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = __vk_dbg_callback;

    PFN_vkCreateDebugUtilsMessengerEXT f =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
    GDF_ASSERT_MSG((f) != NULL, "Function returned was NULL.");
    VK_ASSERT(f(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));
    LOG_DEBUG("Vulkan debugger created.");
#endif
    
    /* ======================================== */
    /* ----- ENUMERATE PHYSICAL DEVICE INFORMATION ----- */
    /* ======================================== */
    u32 physical_device_count = 0;
    VK_ASSERT(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, NULL));
    if (physical_device_count == 0)
    {
        LOG_FATAL("There are no devices supporting vulkan on your system.");
        return false;
    }
    VkPhysicalDevice physical_devices[physical_device_count];
    VK_ASSERT(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, physical_devices));

    /* ======================================== */
    /* ----- CREATE WINDOW SURFACE ----- */
    /* ======================================== */
    GDF_VK_CreateSurface(&context);

    context.physical_device_info_list = GDF_LIST_Create(vk_physical_device);
    for (u32 i = 0; i < physical_device_count; i++)
    {
        vk_physical_device info = {
            .handle = physical_devices[i],
            .usable = true
        };
        // fill properties, features, and memoryinfo field
        vkGetPhysicalDeviceProperties(physical_devices[i], &info.properties);
        vkGetPhysicalDeviceFeatures(physical_devices[i], &info.features);
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &info.memory);
        // fill the swapchain support info field
        // TODO!
        __query_swapchain_support(info.handle, context.surface, &info.sc_support_info);
        // fill the queue info field
        __get_queue_indices(&context, info.handle, &info.queues);
        // TODO! may be a memory bug here check it out later
        GDF_LIST_Push(context.physical_device_info_list, info);
    }

    __filter_available_devices(context.physical_device_info_list);

    /* ======================================== */
    /* ----- DEVICE SELECTION ----- */
    /* ======================================== */

    vk_physical_device* selected_physical_device = NULL;
    u32 device_num = GDF_LIST_GetLength(context.physical_device_info_list);
    for (u32 i = 0; i < device_num; i++)
    {
        if (context.physical_device_info_list[i].usable)
        {
            selected_physical_device = &context.physical_device_info_list[i];
        }
    }

    if (selected_physical_device == NULL)
    {
        LOG_FATAL("Could not find a device to use.");
        return false;
    }

    /* ======================================== */
    /* ----- FIND QUEUE INDICES ----- */
    /* ======================================== */

    bool present_shares_graphics_queue = selected_physical_device->queues.graphics_family_index == selected_physical_device->queues.present_family_index;
    bool transfer_shares_graphics_queue = selected_physical_device->queues.graphics_family_index == selected_physical_device->queues.transfer_family_index;
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
    indices[index++] = selected_physical_device->queues.graphics_family_index;
    if (!present_shares_graphics_queue) {
        indices[index++] = selected_physical_device->queues.present_family_index;
    }
    if (!transfer_shares_graphics_queue) {
        indices[index++] = selected_physical_device->queues.transfer_family_index;
    }

    /* ======================================== */
    /* ----- CREATE DEVICE & FETCH QUEUES ----- */
    /* ======================================== */

    VkDeviceQueueCreateInfo queue_create_infos[index_count];
    for (u32 i = 0; i < index_count; ++i) {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = indices[i];
        queue_create_infos[i].queueCount = 1;
        if (indices[i] == selected_physical_device->queues.graphics_family_index) {
            queue_create_infos[i].queueCount = 2;
        }
        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = 0;
        f32 queue_priority = 1.0f;
        queue_create_infos[i].pQueuePriorities = &queue_priority;
    }

    VkPhysicalDeviceFeatures device_features = {
        .samplerAnisotropy = VK_TRUE,
        .fillModeNonSolid = VK_TRUE
    };

    VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_create_info.queueCreateInfoCount = index_count;
    device_create_info.pQueueCreateInfos = queue_create_infos;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = 1;
    const char* extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    device_create_info.ppEnabledExtensionNames = &extension_names;

    context.device.physical_info = selected_physical_device;

    VK_ASSERT(
        vkCreateDevice(
            selected_physical_device->handle,
            &device_create_info,
            context.allocator,
            &context.device.handle
        )
    );

    LOG_DEBUG("Created device.");

    vkGetDeviceQueue(
        context.device.handle,
        context.device.physical_info->queues.graphics_family_index,
        0,
        &context.device.graphics_queue
    );

    vkGetDeviceQueue(
        context.device.handle,
        context.device.physical_info->queues.present_family_index,
        0,
        &context.device.present_queue
    );

    vkGetDeviceQueue(
        context.device.handle,
        context.device.physical_info->queues.transfer_family_index,
        0,
        &context.device.transfer_queue
    );
    
    LOG_DEBUG("Got queues");

    /* ======================================== */
    /* ----- CREATE COMMAND POOL FOR GRAPHICS QUEUE ----- */
    /* ======================================== */

    VkCommandPoolCreateInfo pool_create_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool_create_info.queueFamilyIndex = context.device.physical_info->queues.graphics_family_index;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_ASSERT(
        vkCreateCommandPool(
            context.device.handle,
            &pool_create_info,
            context.allocator,
            &context.device.graphics_cmd_pool
        )
    );
    LOG_DEBUG("graphics command pool created.");

    /* ======================================== */
    /* ----- CREATE SWAPCHAIN ----- */
    /* ======================================== */
    // TODO! add a lot more checks during swapchain creation
    VkSwapchainCreateInfoKHR sc_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .surface = context.surface,
        .imageArrayLayers = 1
    };
    sc_create_info.clipped = VK_TRUE;
    VkExtent2D extent = {
        .width = context.framebuffer_width,
        .height = context.framebuffer_height
    };
    sc_create_info.imageExtent = extent;
    // for now if these arent supported FUCK YOU
    // ! also initiliaze formats here 
    context.formats.depth_format = vk_utils_find_supported_depth_format(context.device.physical_info->handle);
    if (context.formats.depth_format == VK_FORMAT_UNDEFINED) {
        LOG_ERR("Could not find a supported depth format. Cannot continue program.");
        return false;
    }
    context.formats.image_format = VK_FORMAT_B8G8R8A8_SRGB;
    context.formats.image_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    sc_create_info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    sc_create_info.imageFormat = context.formats.image_format;
    sc_create_info.imageColorSpace = context.formats.image_color_space;
    sc_create_info.minImageCount = 3;
    
    VK_ASSERT(
        vkCreateSwapchainKHR(
            context.device.handle,
            &sc_create_info,
            context.allocator,
            &context.swapchain
        )
    );

    LOG_DEBUG("Created swapchain.");

    /* ======================================== */
    /* ----- CREATE RENDERPASSES, SHADERS, & GRAPHICS PIPELINE ----- */
    /* ======================================== */

    // vkDestroyDevice(context.device.handle, context.allocator);

    if (!__create_renderpasses_and_pipelines(&context))
    {
        return false;
    }

    LOG_DEBUG("Created graphics pipelines & renderpasses");

    /* ======================================== */
    /* ----- Allocate command buffer ----- */
    /* ======================================== */
    VkCommandBufferAllocateInfo command_buf_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = context.device.graphics_cmd_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    vkAllocateCommandBuffers(context.device.handle, &command_buf_info, &context.device.cmd_buffer);
    LOG_DEBUG("Allocated command buffer");

    LOG_INFO("Finished initialization of vulkan stuff...");

    context.ready_for_use = true;

    return true;
}

void vk_renderer_destroy(renderer_backend* backend) 
{
    vkDeviceWaitIdle(context.device.handle);
    VkDevice device = context.device.handle;
    VkAllocationCallbacks* allocator = context.allocator;
    // Destroy a bunch of pipelines
    vkDestroyPipeline(
        device, 
        context.pipelines.geometry.handle, 
        allocator
    );
    vkDestroyPipeline(
        device, 
        context.pipelines.geometry_wireframe.handle, 
        allocator
    );
    vkDestroyPipelineLayout(
        device,
        context.pipelines.geometry.layout,
        allocator
    );
    
    vkDestroyPipeline(
        device, 
        context.pipelines.lighting.handle, 
        allocator
    );
    vkDestroyPipelineLayout(
        device,
        context.pipelines.lighting.layout,
        allocator
    );

    vkDestroyPipeline(
        device, 
        context.pipelines.post_processing.handle, 
        allocator
    );
    vkDestroyPipelineLayout(
        device,
        context.pipelines.post_processing.layout,
        allocator
    );
    vkDestroyRenderPass(
        device, 
        context.renderpasses.main_pass, 
        allocator
    );

    // Destroy shader modules
    vkDestroyShaderModule(
        device,
        context.shaders.geometry_vert,
        allocator
    );
    vkDestroyShaderModule(
        device,
        context.shaders.geometry_frag,
        allocator
    );
    vkDestroyShaderModule(
        device,
        context.shaders.lighting_vert,
        allocator
    );
    vkDestroyShaderModule(
        device,
        context.shaders.lighting_frag,
        allocator
    );
    vkDestroyShaderModule(
        device,
        context.shaders.post_process_vert,
        allocator
    );
    vkDestroyShaderModule(
        device,
        context.shaders.post_process_frag,
        allocator
    );
    // Destroy swapchain, device and resources
    vkDestroySwapchainKHR(
        device,
        context.swapchain,
        allocator
    );
    vkDestroyCommandPool(
        device,
        context.device.graphics_cmd_pool,
        allocator
    );
    // destroy in the opposite order of creation.
    // destroy buffers
    // vk_buffer_destroy(&context, &context.object_vertex_buffer);
    // vk_buffer_destroy(&context, &context.object_idx_buffer);
    // LOG_DEBUG("Destroying shader and pipelines.,.,.");
    // vk_shader_destroy(&context, &context.default_object_shader);

    // // sync objects
    // for (u8 i = 0; i < context.swapchain.max_frames_in_flight; i++) 
    // {
    //     if (context.image_available_semaphores[i]) 
    //     {
    //         vkDestroySemaphore(
    //             context.device.handle,
    //             context.image_available_semaphores[i],
    //             context.allocator);
    //         context.image_available_semaphores[i] = NULL;
    //     }
    //     if (context.queue_complete_semaphores[i]) 
    //     {
    //         vkDestroySemaphore(
    //             context.device.handle,
    //             context.queue_complete_semaphores[i],
    //             context.allocator);
    //         context.queue_complete_semaphores[i] = NULL;
    //     }
    //     vk_fence_destroy(&context, &context.in_flight_fences[i]);
    // }
    // GDF_LIST_Destroy(context.image_available_semaphores);
    // context.image_available_semaphores = NULL;

    // GDF_LIST_Destroy(context.queue_complete_semaphores);
    // context.queue_complete_semaphores = NULL;

    // GDF_LIST_Destroy(context.in_flight_fences);
    // context.in_flight_fences = NULL;

    // GDF_LIST_Destroy(context.images_in_flight);
    // context.images_in_flight = NULL;

    // // command bufs
    // for (u32 i = 0; i < context.swapchain.image_count; i++) 
    // {
    //     if (context.graphics_cmd_buf_list[i].handle) 
    //     {
    //         vk_cmd_buf_free(
    //             &context,
    //             context.device.graphics_cmd_pool,
    //             &context.graphics_cmd_buf_list[i]
    //         );
    //         context.graphics_cmd_buf_list[i].handle = NULL;
    //     }
    // }
    // GDF_LIST_Destroy(context.graphics_cmd_buf_list);
    // context.graphics_cmd_buf_list = NULL;

    // // framebuffers
    // for (u32 i = 0; i < context.swapchain.image_count; i++) 
    // {
    //     vk_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
    // }

    // // renderpass
    // vk_renderpass_destroy(&context, &context.main_renderpass);

    // // swapchain
    // vk_swapchain_destroy(&context, &context.swapchain);

    // // device
    LOG_DEBUG("Destroying Vulkan device...");
    vkDestroyDevice(context.device.handle, context.allocator);

    LOG_DEBUG("Destroying Vulkan surface...");
    if (context.surface) {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = 0;
    }

    LOG_DEBUG("Destroying Vulkan debugger...");
#ifndef GDF_RELEASE
    if (context.debug_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT f =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
        f(context.instance, context.debug_messenger, context.allocator);
    }
#endif
    LOG_DEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(context.instance, context.allocator);
}

void vk_renderer_resize(renderer_backend* backend, u16 width, u16 height) 
{
    new_framebuf_w = width;
    new_framebuf_h = height;
    context.pending_resize_event = true;
}

bool vk_renderer_begin_frame(renderer_backend* backend, f32 delta_time) 
{
    vk_device* device = &context.device;

    // Check if recreating swap chain and boot out.
    if (!context.ready_for_use) {
        if (!vk_result_is_success(vkDeviceWaitIdle(device->handle))) {
            LOG_ERR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (1) failed");
            return false;
        }
        LOG_DEBUG("Something is happening w the renderer");
        return false;
    }

    // // Check if the framebuffer has been resized. If so, a new swapchain must be created.
    // if (context.pending_resize_event) 
    // {
    //     if (!vk_result_is_success(vkDeviceWaitIdle(device->handle)))
    //     {
    //         LOG_ERR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (2) failed");
    //         return false;
    //     }

    //     // If the swapchain recreation failed (because, for example, the window was minimized),
    //     // boot out before unsetting the flag.
    //     if (!__create_swapchain(backend)) {
    //         return false;
    //     }

    //     LOG_INFO("Resized successfully.");
    //     context.pending_resize_event = false;
    //     return false;
    // }

    // something something sync gpu cpu wait something.
    // and then... 
    VkCommandBuffer cmd_buffer = context.device.cmd_buffer;
    vkResetCommandBuffer(cmd_buffer, 0);

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    vkBeginCommandBuffer(cmd_buffer, &begin_info);

    VkRenderPassBeginInfo pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        , afaw // TODO!
    };
    vkCmdBeginRenderPass(cmd_buffer, )


    return true;
}

bool vk_renderer_end_frame(renderer_backend* backend, f32 delta_time) 
{
    // vk_cmd_buf* command_buffer = &context.graphics_cmd_buf_list[context.img_idx];
    VkCommandBuffer cmd_buffer = context.device.cmd_buffer;

    vkCmdEndRenderPass(cmd_buffer);

    if (vkEndCommandBuffer(cmd_buffer) != VK_SUCCESS) {
        
        return false;
    }

    // vk_renderpass_end(command_buffer, &context.main_renderpass);

    // // end recording
    // vk_cmd_buf_end(command_buffer);

    // // Make sure the previous frame is not using this image (i.e. its fence is being waited on)
    // if (context.images_in_flight[context.img_idx] != VK_NULL_HANDLE) {  // was frame
    //     vk_fence_wait(
    //         &context,
    //         context.images_in_flight[context.img_idx],
    //         UINT64_MAX
    //     );
    // }

    // // Mark the image fence as in-use by this frame.
    // context.images_in_flight[context.img_idx] = &context.in_flight_fences[context.current_frame];

    // // Reset the fence for use on the next frame
    // vk_fence_reset(&context, &context.in_flight_fences[context.current_frame]);

    // // Submit the queue and wait for the operation to complete.
    // // Begin queue submission
    // VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};

    // // Command buffer(s) to be executed.
    // submit_info.commandBufferCount = 1;
    // submit_info.pCommandBuffers = &command_buffer->handle;

    // // The semaphore(s) to be signaled when the queue is complete.
    // submit_info.signalSemaphoreCount = 1;
    // submit_info.pSignalSemaphores = &context.queue_complete_semaphores[context.current_frame];

    // // Wait semaphore ensures that the operation cannot begin until the image is available.
    // submit_info.waitSemaphoreCount = 1;
    // submit_info.pWaitSemaphores = &context.image_available_semaphores[context.current_frame];

    // // Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
    // // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent colour attachment
    // // writes from executing until the semaphore signals (i.e. one frame is presented at a time)
    // VkPipelineStageFlags flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    // submit_info.pWaitDstStageMask = flags;

    // VK_ASSERT(
    //     vkQueueSubmit(
    //         context.device.graphics_queue,
    //         1,
    //         &submit_info,
    //         context.in_flight_fences[context.current_frame].handle
    //     )
    // );

    // vk_cmd_buf_set_submitted(command_buffer);
    // // End queue submission

    // // Give the image back to the swapchain.
    // vk_swapchain_present(
    //     &context,
    //     &context.swapchain,
    //     context.device.graphics_queue,
    //     context.device.present_queue,
    //     context.queue_complete_semaphores[context.current_frame],
    //     context.img_idx
    // );

    return true;
}
