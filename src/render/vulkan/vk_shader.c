#include "vk_shader.h"
#include "core/os/io.h"
#include <stdio.h>

#define SHADER_TYPE_VERT "vert"
#define SHADER_TYPE_FRAG "frag"

static bool create_shader_module(
    vk_context* context,
    const char* name,
    const char* type_str,
    VkShaderStageFlagBits shader_stage_flag,
    u32 stage_index,
    vk_shader_stage* shader_stages) {

    char rel_path[512];
    sprintf(rel_path, "resources/shaders/%s.%s.spv", name, type_str);

    GDF_MemZero(&shader_stages[stage_index].create_info, sizeof(VkShaderModuleCreateInfo));
    shader_stages[stage_index].create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    u64 size;
    char* src_buf;
    if ((src_buf = GDF_ReadFileExactLen(rel_path)) == NULL) {
        LOG_ERR("Unable to read shader module: %s.", rel_path);
        return false;
    }
    shader_stages[stage_index].create_info.codeSize = size;
    shader_stages[stage_index].create_info.pCode = (u32*)src_buf;

    VK_ASSERT(
        vkCreateShaderModule(
            context->device.logical,
            &shader_stages[stage_index].create_info,
            context->allocator,
            &shader_stages[stage_index].handle
        )
    );

    // shader stage info
    GDF_MemZero(&shader_stages[stage_index].shader_stage_create_info, sizeof(VkPipelineShaderStageCreateInfo));
    shader_stages[stage_index].shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[stage_index].shader_stage_create_info.stage = shader_stage_flag;
    shader_stages[stage_index].shader_stage_create_info.module = shader_stages[stage_index].handle;
    shader_stages[stage_index].shader_stage_create_info.pName = "main";

    GDF_Free(src_buf);

    return true;
}

void vk_shader_create(vk_context* context, const char* resource_path, VkShaderStageFlags stage, vk_shader* out_module)
{
    const char src_path[MAX_PATH_LEN + 50];
    GDF_GetResourcePath(resource_path, src_path);

    char* src = GDF_ReadFileExactLen(src_path);
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = strlen(src);
    // this will probably fail diabolically
    create_info.pCode = (u32*)src;
    VK_ASSERT(
        vkCreateShaderModule(
            context->device.logical, &create_info, context->allocator, &out_module->handle
        )
    );
}

void vk_shader_destroy(vk_shader* module)
{
    
}