#include "vk_utils.h"

#include "core/os/io.h"

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
        .pCode = code,
    };

    VkResult res = vkCreateShaderModule(
        context->device.handle,
        &create_info,
        context->allocator,
        out_module
    );

    return res == VK_SUCCESS;
}