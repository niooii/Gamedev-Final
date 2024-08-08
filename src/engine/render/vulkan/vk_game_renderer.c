#include "vk_game_renderer.h"

bool vk_draw_game(vk_renderer_context* context, renderer_backend* backend, u8 resource_idx, f32 dt)
{
    GDF_Game* game = backend->game;
    GDF_Camera* active_camera = game->main_camera;
    VkCommandBuffer cmd_buffer = context->command_buffers[resource_idx];

    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &context->up_facing_plane_vbo.handle, offsets);
    vkCmdBindIndexBuffer(cmd_buffer, context->up_facing_plane_index_buffer.handle, 0, VK_INDEX_TYPE_UINT16);
    
    // Render chunks
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context->block_pipeline.handle);

    VkDescriptorSet sets[1] = {
        context->ubo_descriptor_sets[resource_idx],

    };

    vkCmdBindDescriptorSets(
        cmd_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        context->block_pipeline.layout,
        0,
        sizeof(sets) / sizeof(VkDescriptorSet),
        sets, 
        0, 
        NULL
    );

    // TODO!
    // u32 num_visible_chunks = GDF_LIST_GetLength(context->visible_chunks);
    // for (u32 i = 0; i < num_visible_chunks; i++) 
    // {
    //     GpuChunkData* data = &context->visible_chunks[i];
    //     i32 new_vals[3] = {
    //         data->chunk_x,
    //         data->chunk_y,
    //         data->chunk_z,
    //     };
    //     vkCmdPushConstants(
    //         cmd_buffer,
    //         context->grid_pipeline.layout,
    //         VK_SHADER_STAGE_VERTEX_BIT,
    //         0,
    //         sizeof(i32) * 3,
    //         new_vals
    //     );
        
        
    // }

    // vkCmdDrawIndexed()

    // draw debug grid
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context->grid_pipeline.handle);

    vkCmdBindDescriptorSets(
        cmd_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        context->grid_pipeline.layout,
        0,
        1,
        &context->ubo_descriptor_sets[resource_idx], 
        0, 
        NULL
    );

    vkCmdPushConstants(
        cmd_buffer,
        context->grid_pipeline.layout,
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(vec3),
        &active_camera->pos
    );

    vkCmdDrawIndexed(cmd_buffer, 6, 1, 0, 0, 0);
    return true;
}