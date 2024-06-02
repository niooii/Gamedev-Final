#include "vk_renderer.h"
#include "vk_types.h"
#include "core/containers/list.h"
#include "vk_os.h"

static vk_context context;
static u32 tmp_framebuf_w;
static u32 tmp_framebuf_h;

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

// filters the context.physical_device_info list 
static void __filter_available_devices()
{
    u32 list_len = GDF_LIST_GetLength(context.physical_device_info_list);
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
            device_suitable = false;
            goto filter_device_skip;
        }    
        // TODO! Check if the device supports
        // all the required extensions

        filter_device_skip:
        if (!device_suitable)
        {
            LOG_DEBUG("Marked device \'%s\' as unusable.", device->properties.deviceName);
            device->usable = false;
        }
    }
}

void __create_cmd_bufs(renderer_backend* backend)
{
    if (context.graphics_cmd_buf_list == NULL) 
    {
        context.graphics_cmd_buf_list = GDF_LIST_Reserve(vk_cmd_buf, context.swapchain.image_count);
        for (u32 i = 0; i < context.swapchain.image_count; i++) 
        {
            GDF_MemZero(&context.graphics_cmd_buf_list[i], sizeof(vk_cmd_buf));
        }
    }

    for (u32 i = 0; i < context.swapchain.image_count; ++i) 
    {
        if (context.graphics_cmd_buf_list[i].handle) {
            vk_cmd_buf_free(
                &context,
                context.device.graphics_cmd_pool,
                &context.graphics_cmd_buf_list[i]
            );
        }
        GDF_MemZero(&context.graphics_cmd_buf_list[i], sizeof(vk_cmd_buf));
        vk_cmd_buf_allocate(
            &context,
            context.device.graphics_cmd_pool,
            TRUE,
            &context.graphics_cmd_buf_list[i]
        );
    }

    LOG_DEBUG("Vulkan command buffers created.");
}

void __regenerate_framebuffers(renderer_backend* backend, vk_swapchain* swapchain, vk_renderpass* renderpass) {
    for (u32 i = 0; i < swapchain->image_count; i++) 
    {
        // TODO! make this dynamic based on the currently configured attachments
        u32 attachment_count = 2;
        VkImageView attachments[] = {
            swapchain->views[i],
            swapchain->depth_attachment.view};

        vulkan_framebuffer_create(
            &context,
            renderpass,
            context.framebuffer_width,
            context.framebuffer_height,
            attachment_count,
            attachments,
            &context.swapchain.framebuffers[i]);
    }
}

// ===== FORWARD DECLARATIONS END =====
bool vk_renderer_init(renderer_backend* backend, const char* application_name) 
{
    // TODO! custom allocator.
    context.allocator = 0;
    
    GDF_GetWindowSize(&tmp_framebuf_w, &tmp_framebuf_h);
    context.framebuffer_width = tmp_framebuf_w;
    context.framebuffer_height = tmp_framebuf_h;

    tmp_framebuf_w = 0;
    tmp_framebuf_h = 0;


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
    
    // enumerate physical devices then create logical 
    u32 physical_device_count = 0;
    VK_ASSERT(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, NULL));
    if (physical_device_count == 0)
    {
        LOG_FATAL("There are no devices supporting vulkan on your system.");
        return false;
    }
    VkPhysicalDevice physical_devices[physical_device_count];
    VK_ASSERT(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, physical_devices));

    // test surface creation remove later and place elsewhwere maybe
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
        vk_device_query_swapchain_support(info.handle, context.surface, &info.sc_support_info);
        // fill the queue info field
        vk_device_query_queues(&context, info.handle, &info.queues);
        // TODO! may be a memory bug here check it out later
        GDF_LIST_Push(context.physical_device_info_list, info);
    }

    __filter_available_devices();
    vk_device_create(&context, NULL);

    vk_swapchain_create(
        &context,
        context.framebuffer_width,
        context.framebuffer_height,
        &context.swapchain
    );

    vk_renderpass_create(
        &context,
        &context.main_renderpass,
        0, 0, context.framebuffer_width, context.framebuffer_height,
        0.0f, 0.0f, 0.2f, 1.0f,
        1.0f,
        0
    );

    context.swapchain.framebuffers = GDF_LIST_Reserve(vk_framebuffer, context.swapchain.image_count);
    __regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);

    __create_cmd_bufs(backend);


    LOG_INFO("Finished initialization of vulkan stuff...");

    return true;
}

void vk_renderer_destroy(renderer_backend* backend) 
{
    // destroy in the opposite order of creation.

    for (u32 i = 0; i < context.swapchain.image_count; i++) 
    {
        if (context.graphics_cmd_buf_list[i].handle) 
        {
            vk_cmd_buf_free(
                &context,
                context.device.graphics_cmd_pool,
                &context.graphics_cmd_buf_list[i]
            );
            context.graphics_cmd_buf_list[i].handle = NULL;
        }
    }
    GDF_LIST_Destroy(context.graphics_cmd_buf_list);
    context.graphics_cmd_buf_list = NULL;

    for (u32 i = 0; i < context.swapchain.image_count; i++) 
    {
        vk_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
    }

    vk_renderpass_destroy(&context, &context.main_renderpass);

    vk_swapchain_destroy(&context, &context.swapchain);

    LOG_DEBUG("Destroying Vulkan device...");
    vk_device_destroy(&context);

    LOG_DEBUG("Destroying Vulkan surface...");
    if (context.surface) {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = 0;
    }

    LOG_DEBUG("Destroying Vulkan debugger...");
    if (context.debug_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT f =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
        f(context.instance, context.debug_messenger, context.allocator);
    }
    LOG_DEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(context.instance, context.allocator);
}

void vk_renderer_resize(renderer_backend* backend, u16 width, u16 height) 
{

}

bool vk_renderer_begin_frame(renderer_backend* backend, f32 delta_time) 
{
    return true;
}

bool vk_renderer_end_frame(renderer_backend* backend, f32 delta_time) 
{
    return true;
}
