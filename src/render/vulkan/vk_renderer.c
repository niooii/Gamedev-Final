#include "vk_renderer.h"
#include "vk_types.h"
#include "core/containers/list.h"
#include "vk_os.h"

static vk_context context;

// filters the context.physical_device_info list 
static void filter_available_devices()
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

bool vk_renderer_init(renderer_backend* backend, const char* application_name) 
{
    // TODO! custom allocator.
    context.allocator = 0;

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
    
    // enumerate physical devices then create logical 
    u32 physical_device_count = 0;
    VK_ASSERT(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, NULL));
    if (physical_device_count == 0)
    {
        LOG_FATAL("There are no devices supporting vulkan on your system.");
        return false;
    }
    VkPhysicalDevice* physical_devices[physical_device_count];
    VK_ASSERT(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, physical_devices));
    VkPhysicalDeviceProperties device_properties[physical_device_count];

    for (u32 i = 0; i < physical_device_count; i++)
    {
        vkGetPhysicalDeviceProperties(physical_devices[i], &device_properties[i]);
    }

    // test surface creation remove later and place elsewhwere maybe
    GDF_VK_CreateSurface(&context);

    // TODO! setup debugger util extension callback thingy

    context.physical_device_info_list = GDF_LIST_Create(vk_physical_device);
    for (u32 i = 0; i < physical_device_count; i++)
    {
        vk_physical_device info = {
            .handle = physical_devices[i],
            .properties = device_properties[i],
            .usable = true
        };
        // fill the swapchain support info field
        vk_device_query_swapchain_support(info.handle, context.surface, &info.sc_support_info);
        // fill the queue info field
        vk_device_query_queues(&context, info.handle, &info.queues);
        // TODO! may be a memory bug here check it out later
        GDF_LIST_Push(context.physical_device_info_list, info);
    }

    filter_available_devices();

    return true;
}

void vk_renderer_shutdown(renderer_backend* backend) 
{

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