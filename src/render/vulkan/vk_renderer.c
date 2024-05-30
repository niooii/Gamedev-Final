#include "vk_renderer.h"
#include "vk_types.h"
#include "core/containers/list.h"

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

    // TODO! platform specific extensions
    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames = 0;

    // validation layers
    const char** validation_layers = GDF_LIST_Create(const char*);
    const char* khr_validation = "VK_LAYER_KHRONOS_validation";
    GDF_LIST_Push(validation_layers, khr_validation);
    create_info.enabledLayerCount = GDF_LIST_GetLength(validation_layers);
    create_info.ppEnabledLayerNames = validation_layers;

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