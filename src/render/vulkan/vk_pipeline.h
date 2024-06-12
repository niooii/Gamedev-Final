#pragma once

#include "core.h"

#include "vk_types.h"

void vk_pipeline_create_graphics(
    vk_context* context,
    vk_renderpass* renderpass,
    u32 attribute_count,
    VkVertexInputAttributeDescription* attributes,
    u32 descriptor_set_layout_count,
    VkDescriptorSetLayout* descriptor_set_layouts,
    u32 stage_count,
    VkPipelineShaderStageCreateInfo* stages,
    VkViewport viewport,
    VkRect2D scissor,
    bool is_wireframe,
    vk_pipeline* out_pipeline);

void vk_pipeline_create_compute(vk_pipeline* out_pipeline); 
void vulkan_pipeline_destroy(vk_context* context, vk_pipeline* pipeline);

void vulkan_pipeline_bind(vk_cmd_buf* command_buffer, VkPipelineBindPoint bind_point, vk_pipeline* pipeline);