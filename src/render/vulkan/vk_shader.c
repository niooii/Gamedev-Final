#include "vk_shader.h"
#include "core/os/io.h"
#include <stdio.h>
#include "math/math_types.h"
#include "vk_pipeline.h"

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

// TODO! hardcoded
#define STAGE_COUNT 2
bool vk_shader_create(vk_context* context, const char* resource_path, vk_shader* out_shader)
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

    // TODO: Descriptors

    // pipeline creation
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = (f32)context->framebuffer_height;
    viewport.width = (f32)context->framebuffer_width;
    viewport.height = -(f32)context->framebuffer_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context->framebuffer_width;
    scissor.extent.height = context->framebuffer_height;

    // Attributes
    u32 offset = 0;
    const i32 attribute_count = 1;
    VkVertexInputAttributeDescription attribute_descriptions[attribute_count];
    // Position
    VkFormat formats[attribute_count] = {
        VK_FORMAT_R32G32B32_SFLOAT
    };
    u64 sizes[attribute_count] = {
        sizeof(vec3)
    };
    for (u32 i = 0; i < attribute_count; i++) 
    {
        attribute_descriptions[i].binding = 0;   // binding index - should match binding desc
        attribute_descriptions[i].location = i;  // attrib location
        attribute_descriptions[i].format = formats[i];
        attribute_descriptions[i].offset = offset;
        offset += sizes[i];
    }

    // TODO: Desciptor set layouts.

    // NOTE: should match the number of shader->stages.
    VkPipelineShaderStageCreateInfo stage_create_infos[STAGE_COUNT];
    GDF_MemZero(stage_create_infos, sizeof(stage_create_infos));
    for (u32 i = 0; i < STAGE_COUNT; i++) 
    {
        stage_create_infos[i].sType = out_shader->stages[i].shader_stage_create_info.sType;
        stage_create_infos[i] = out_shader->stages[i].shader_stage_create_info;
    }

    if (
        !vk_pipeline_create_graphics(
            context,
            &context->main_renderpass,
            attribute_count,
            attribute_descriptions,
            0,
            0,
            STAGE_COUNT,
            stage_create_infos,
            viewport,
            scissor,
            false,
            &out_shader->pipeline
        )
    ) 
    {
        LOG_ERR("Failed to load graphics pipeline for default shader.");
        return false;
    }

    return true;
}

void vk_shader_destroy(vk_context* context, vk_shader* shader)
{
    vk_pipeline_destroy(context, &shader->pipeline);

    for (u32 i = 0; i < STAGE_COUNT; i++) 
    {
        vkDestroyShaderModule(context->device.logical, shader->stages[i].handle, context->allocator);
        shader->stages[i].handle = 0;
    }
}
void vk_shader_use(vk_context* context, vk_shader* shader)
{
    u32 image_index = context->img_idx;
    vk_pipeline_bind(
        &context->graphics_cmd_buf_list[image_index], 
        VK_PIPELINE_BIND_POINT_GRAPHICS, 
        &shader->pipeline
    );
}