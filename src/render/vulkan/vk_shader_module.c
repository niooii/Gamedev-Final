#include "vk_shader_module.h"

void vk_shader_module_create(vk_context* context, const char* resource_path, VkShaderStageFlags stage, vk_shader_module* out_module)
{
    const char src_path[MAX_PATH_LEN + 50];
    GDF_GetResourcePath(resource_path, src_path);

    char* src = GDF_ReadFileExactLen(src_path);
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = strlen(src);
    // this will probably fail diabolically
    create_info.pCode = src;
    VK_ASSERT(
        vkCreateShaderModule(
            context->device.logical, &create_info, context->allocator, &out_module->handle
        )
    );

    out_module
}

void vk_shader_module_destroy(vk_shader_module* module)
{

}