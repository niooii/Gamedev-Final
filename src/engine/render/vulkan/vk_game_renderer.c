#include "vk_game_renderer.h"
#include "engine/geometry.h"

Transform random_ahh_cube;

bool vk_draw_game(vk_renderer_context* context, Renderer* backend, u8 resource_idx, f32 dt)
{
    // TODO! BEGONE
    transform_init_default(&random_ahh_cube);
    random_ahh_cube.pos = vec3_new(0.5, 0.5, 0.5);
    transform_recalc_model_matrix(&random_ahh_cube);
    
    GDF_Game* game = backend->game;
    GDF_Camera* active_camera = game->main_camera;
    VkCommandBuffer cmd_buffer = context->command_buffers[resource_idx];

    VkDeviceSize offsets[] = {0};
    
    // Render chunks

    VkDescriptorSet sets[2] = {
        context->global_vp_ubo_sets[resource_idx],
        context->block_pipeline.descriptor_sets[resource_idx]
    };

    VkPipeline block_pipeline_layout = VK_NULL_HANDLE;
    VkPipelineLayout block_pipeline = VK_NULL_HANDLE;
    switch (backend->render_mode) 
    {
        case GDF_RENDER_MODE_FULL:
        {
            block_pipeline = context->block_pipeline.handle;
            block_pipeline_layout = context->block_pipeline.layout;
            break;
        }
        case GDF_RENDER_MODE_WIREFRAME:
        {
            block_pipeline = context->block_pipeline.wireframe_handle;
            block_pipeline_layout = context->block_pipeline.wireframe_layout;
            break;
        }
        default:
        LOG_FATAL("wyd lil bro");
        logging_flush_buffer();
        exit(1);
    }
    
    vkCmdBindPipeline(
        cmd_buffer, 
        VK_PIPELINE_BIND_POINT_GRAPHICS, 
        block_pipeline
    );

    vkCmdBindDescriptorSets(
        cmd_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        block_pipeline_layout,
        0,
        sizeof(sets) / sizeof(VkDescriptorSet),
        sets, 
        0, 
        NULL
    );

    vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &context->cube_vbo.handle, offsets);
    vkCmdBindIndexBuffer(cmd_buffer, context->cube_index_buffer.handle, 0, VK_INDEX_TYPE_UINT16);
    struct PushConstantTemp {
        mat4 model;
        u32 block_type;
    };
    struct PushConstantTemp pct = {
        .model = random_ahh_cube.model_matrix,
        .block_type = GDF_BLOCKTYPE_Stone
    };
    vkCmdPushConstants(
        cmd_buffer, block_pipeline_layout, 
        VK_SHADER_STAGE_VERTEX_BIT,
        0, sizeof(struct PushConstantTemp), &pct
    );

    vkCmdDrawIndexed(cmd_buffer, 36, 1, 0, 0, 0);

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

    // draw debug grid
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context->grid_pipeline.handle);

    vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &context->up_facing_plane_vbo.handle, offsets);
    vkCmdBindIndexBuffer(cmd_buffer, context->up_facing_plane_index_buffer.handle, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(
        cmd_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        context->grid_pipeline.layout,
        0,
        1,
        &context->global_vp_ubo_sets[resource_idx], 
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