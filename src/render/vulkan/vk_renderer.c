#include "vk_renderer.h"
#include "vk_types.h"
#include "core/containers/list.h"
#include "vk_os.h"

static vk_context context;

bool vk_renderer_init(renderer_backend* backend, const char* application_name) {
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
    VkResult result = vkCreateInstance(&create_info, context.allocator, &context.instance);
    if (result != VK_SUCCESS) {
        LOG_ERR("vkCreateInstance failed with result: %u", result);
        return false;
    }

    LOG_INFO("Vulkan instance initialized successfully.");
    return true;
}

void vk_renderer_shutdown(renderer_backend* backend) {
}

void vk_renderer_resize(renderer_backend* backend, u16 width, u16 height) {
}

bool vk_renderer_begin_frame(renderer_backend* backend, f32 delta_time) {
    return true;
}

bool vk_renderer_end_frame(renderer_backend* backend, f32 delta_time) {
    return true;
}