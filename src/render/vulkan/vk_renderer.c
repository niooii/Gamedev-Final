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

//     const u64 vertex_buffer_size = sizeof(vertex_3d) * 1024 * 1024;
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

    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE; 

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
    context.image_format = VK_FORMAT_B8G8R8A8_SRGB;
    context.image_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    sc_create_info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    sc_create_info.imageFormat = context.image_format;
    sc_create_info.imageColorSpace = context.image_color_space;
    sc_create_info.minImageCount = 3;
    
    VK_ASSERT(
        vkCreateSwapchainKHR(
            context.device.handle,
            &sc_create_info,
            context.allocator,
            &context.swapchain
        )
    );

    /* ======================================== */
    /* ----- CREATE RENDERPASS AND SUBPASSES ----- */
    /* ======================================== */
    
    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS
    };

    u32 attachment_counts = 2;
    VkAttachmentDescription attachments[2];

    VkAttachmentDescription color_attachment;
    color_attachment.format = context.image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;      
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  
    color_attachment.flags = 0;

    attachments[0] = color_attachment;

    VkAttachmentReference color_attachment_ref = {
        // this is the index in the "attachments" array
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    // TODO! other attachments

    VkRenderPassCreateInfo rp_create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    };

    VkRenderPass main_pass;

    // TODO! fill info struct

    VK_ASSERT(
        vkCreateRenderPass(
            context.device.handle,
            &rp_create_info,
            context.allocator,
            &main_pass
        )
    );

    /* ======================================== */
    /* ----- CREATE SHADERS & GRAPHICS PIPELINE ----- */
    /* ======================================== */
    VkGraphicsPipelineCreateInfo gp_full_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .renderPass = main_pass
    };

    // load vertex shader
    VkShaderModule vert;
    if (!vk_utils_create_shader_module(&context, "resources/shaders/default.vert.spv", &vert)) {
        LOG_ERR("Failed to create default vertex shader. exiting...");
        return false;
    }
    VkShaderModule frag;
    if (!vk_utils_create_shader_module(&context, "resources/shaders/default.frag.spv", &frag)) {
        LOG_ERR("Failed to create default fragment shader. exiting...");
        return false;
    }
    VkPipelineShaderStageCreateInfo vertex_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vert,
        .pName = "default vert"
    };
    VkPipelineShaderStageCreateInfo frag_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = frag,
        .pName = "default frag"
    };
    VkPipelineShaderStageCreateInfo shader_stages[2] = {
        vertex_info,
        frag_info
    };
    gp_full_create_info.stageCount = 2;
    gp_full_create_info.pStages = shader_stages;

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
    };

    input_assembly.primitiveRestartEnable = VK_FALSE;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

    gp_full_create_info.pInputAssemblyState = &input_assembly;

    VK_ASSERT(
        vkCreateGraphicsPipelines(
            context.device.handle,
            VK_NULL_HANDLE,
            1,
            &gp_full_create_info,
            context.allocator,
            &context.graphics_full_pipeline
        )
    );

    LOG_DEBUG("Created graphics pipelines");

    LOG_INFO("Finished initialization of vulkan stuff...");

    return true;
}

void vk_renderer_destroy(renderer_backend* backend) 
{
    vkDeviceWaitIdle(context.device.handle);
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
    // if (context.creating_swapchain) {
    //     if (!vk_result_is_success(vkDeviceWaitIdle(device->handle))) {
    //         LOG_ERR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (1) failed");
    //         return false;
    //     }
    //     LOG_DEBUG("Swapchain is being recreated rn..");
    //     return false;
    // }

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

    // // Wait for the execution of the current frame to complete. The fence being free will allow this one to move on.
    // if (
    //     !vk_fence_wait(
    //         &context,
    //         &context.in_flight_fences[context.current_frame],
    //         UINT64_MAX
    //         )
    //     ) 
    // {
    //     LOG_WARN("In-flight fence wait failure!");
    //     return false;
    // }

    // // Acquire the next image from the swap chain. Pass along the semaphore that should signaled when this completes.
    // // This same semaphore will later be waited on by the queue submission to ensure this image is available.
    // if (!vk_swapchain_acquire_next_image_index(
    //         &context,
    //         &context.swapchain,
    //         UINT64_MAX,
    //         context.image_available_semaphores[context.current_frame],
    //         0,
    //         &context.img_idx)) {
    //     return false;
    // }

    // // Begin recording commands.
    // vk_cmd_buf* command_buffer = &context.graphics_cmd_buf_list[context.img_idx];
    // vk_cmd_buf_reset(command_buffer);
    // vk_cmd_buf_begin(command_buffer, false, false, false);

    // // make as similar to opengls as possible
    // VkViewport viewport;
    // viewport.x = 0.0f;
    // viewport.y = (f32)context.framebuffer_height;
    // viewport.width = (f32)context.framebuffer_width;
    // viewport.height = -(f32)context.framebuffer_height;
    // viewport.minDepth = 0.0f;
    // viewport.maxDepth = 1.0f;

    // // scissor
    // VkRect2D scissor;
    // scissor.offset.x = scissor.offset.y = 0;
    // scissor.extent.width = context.framebuffer_width;
    // scissor.extent.height = context.framebuffer_height;

    // vkCmdSetViewport(command_buffer->handle, 0, 1, &viewport);
    // vkCmdSetScissor(command_buffer->handle, 0, 1, &scissor);

    // context.main_renderpass.w = context.framebuffer_width;
    // context.main_renderpass.h = context.framebuffer_height;

    // vk_renderpass_begin(
    //     command_buffer,
    //     &context.main_renderpass,
    //     context.swapchain.framebuffers[context.img_idx].handle
    // );

    // // TODO! remove later
    // vk_shader_use(&context, &context.default_object_shader);

    // VkDeviceSize offsets[1] = {0};
    // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &context.object_vertex_buffer.handle, (VkDeviceSize*)offsets);

    // vkCmdBindIndexBuffer(command_buffer->handle, context.object_idx_buffer.handle, 0, VK_INDEX_TYPE_UINT32);

    // vkCmdDrawIndexed(command_buffer->handle, 6, 1, 0, 0, 0);
    // ok dont remove anything else

    return true;
}

bool vk_renderer_end_frame(renderer_backend* backend, f32 delta_time) 
{
    // vk_cmd_buf* command_buffer = &context.graphics_cmd_buf_list[context.img_idx];

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
