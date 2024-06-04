#include "vk_cmd_buf.h"

void vk_cmd_buf_allocate(
    vk_context* context,
    VkCommandPool pool,
    bool is_primary,
    vk_cmd_buf* out_cmd_buf)
{
    GDF_MemZero(out_cmd_buf, sizeof(*out_cmd_buf));

    VkCommandBufferAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocate_info.commandPool = pool;
    allocate_info.level = is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocate_info.commandBufferCount = 1;
    allocate_info.pNext = 0;

    out_cmd_buf->state = CMD_BUF_STATE_NOT_ALLOCATED;
    VK_ASSERT(vkAllocateCommandBuffers(
        context->device.logical,
        &allocate_info,
        &out_cmd_buf->handle));
    out_cmd_buf->state = CMD_BUF_STATE_READY;
}

void vk_cmd_buf_free(
    vk_context* context,
    VkCommandPool pool,
    vk_cmd_buf* cmd_buf)
{
    vkFreeCommandBuffers(
        context->device.logical,
        pool,
        1,
        &cmd_buf->handle
    );

    cmd_buf->handle = 0;
    cmd_buf->state = CMD_BUF_STATE_NOT_ALLOCATED;
}

void vk_cmd_buf_begin(
    vk_cmd_buf* cmd_buf,
    bool is_single_use,
    bool is_renderpass_continue,
    bool is_simultaneous_use)
{
    VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin_info.flags = 0;
    if (is_single_use) {
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }
    if (is_renderpass_continue) {
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    }
    if (is_simultaneous_use) {
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    }

    VK_ASSERT(vkBeginCommandBuffer(cmd_buf->handle, &begin_info));
    cmd_buf->state = CMD_BUF_STATE_RECORDING;
}

void vk_cmd_buf_end(vk_cmd_buf* cmd_buf)
{
    VK_ASSERT(vkEndCommandBuffer(cmd_buf->handle));
    cmd_buf->state = CMD_BUF_STATE_RECORDING_ENDED;
}

void vk_cmd_buf_set_submitted(vk_cmd_buf* cmd_buf)
{
    cmd_buf->state = CMD_BUF_STATE_SUBMITTED;
}

void vk_cmd_buf_reset(vk_cmd_buf* cmd_buf)
{
    cmd_buf->state = CMD_BUF_STATE_READY;
}

void vk_cmd_buf_allocate_and_begin_single_use(
    vk_context* context,
    VkCommandPool pool,
    vk_cmd_buf* out_cmd_buf)
{
    vk_cmd_buf_allocate(context, pool, true, out_cmd_buf);
    vk_cmd_buf_begin(out_cmd_buf, true, false, false);
}

void vk_cmd_buf_end_single_use(
    vk_context* context,
    VkCommandPool pool,
    vk_cmd_buf* cmd_buf,
    VkQueue queue)
{
    // End the command buffer.
    vk_cmd_buf_end(cmd_buf);

    // Submit the queue
    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buf->handle;
    VK_ASSERT(vkQueueSubmit(queue, 1, &submit_info, 0));

    // Wait for it to finish
    VK_ASSERT(vkQueueWaitIdle(queue));

    // Free the command buffer.
    vk_cmd_buf_free(context, pool, cmd_buf);
}