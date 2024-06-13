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
    LOG_DEBUG("??? %s", rel_path);

    GDF_MemZero(&shader_stages[stage_index].create_info, sizeof(VkShaderModuleCreateInfo));
    shader_stages[stage_index].create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    u8* src_buf;
    u64 src_len;
    if ((src_buf = GDF_ReadBytesExactLen(rel_path, &src_len)) == NULL) {
        LOG_ERR("Unable to read shader module: %s.", rel_path);
        return false;
    }
    shader_stages[stage_index].create_info.codeSize = src_len;
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

void vk_shader_create(vk_context* context, const char* resource_path, vk_shader* out_shader)
{
	// TODO! hardcoded for now 
	const char* default_name = "default";

	if (
		!create_shader_module(
			context,
			default_name,
			"vert",
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			out_shader->stages
		)
	)
    {
        LOG_ERR("failed to create default vertex shader might wanna look into that.");
    }

    if (
		!create_shader_module(
			context,
			default_name,
			"frag",
			VK_SHADER_STAGE_FRAGMENT_BIT,
			1,
			out_shader->stages
		)
	)
    {
        LOG_ERR("failed to create default frag shader might wanna look into that.");
    }
}

void vk_shader_destroy(vk_context* context, vk_shader* shader)
{

}
void vk_shader_use(vk_context* context, vk_shader* shader)
{

}