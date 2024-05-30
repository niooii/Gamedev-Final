#include "vk_renderer.h"
#include "vk_types.h"

static vk_context context;

bool vk_renderer_init(renderer_backend* backend, const char* application_name) {
    // TODO! custom allocator.
    context.allocator = 0;

    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Kohi Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames = 0;
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = 0;

    VkResult result = vkCreateInstance(&create_info, context.allocator, &context.instance);
    if(result != VK_SUCCESS) {
        LOG_ERR("vkCreateInstance failed with result: %u", result);
        return false;
    }

    LOG_INFO("Vulkan renderer initialized successfully.");
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