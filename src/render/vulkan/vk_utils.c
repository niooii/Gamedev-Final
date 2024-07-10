#include "vk_utils.h"

#include "core/os/io.h"

VkFormat vk_utils_find_supported_depth_format(VkPhysicalDevice physical_device) 
{
    VkFormat depth_formats[] = 
    {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };
    u32 format_count = sizeof(depth_formats) / sizeof(depth_formats[0]);

    for (u32 i = 0; i < format_count; i++) 
    {
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(physical_device, depth_formats[i], &format_properties);

        if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
        {
            return depth_formats[i];
        }
    }

    return VK_FORMAT_UNDEFINED;
}

bool vk_utils_create_shader_module(vk_renderer_context* context, const char* src_rel_path, VkShaderModule* out_module) 
{
    u64 src_size = GDF_GetFileSize(src_rel_path);
    char code[src_size];
    GDF_MemZero(code, src_size);
    if (!GDF_ReadFile(src_rel_path, code, src_size)) 
    {
        LOG_ERR("Failed to read file: %s", src_rel_path);
        return false;
    }

    VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = src_size,
        .pCode = ((u32*)code),
    };

    VkResult res = vkCreateShaderModule(
        context->device.handle,
        &create_info,
        context->allocator,
        out_module
    );

    return res == VK_SUCCESS;
}