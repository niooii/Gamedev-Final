#pragma once

#include "vk_types.h"

void vk_cmd_buf_allocate(
    vk_context* context,
    VkCommandPool pool,
    bool is_primary,
    vk_cmd_buf* out_cmd_buf
);

// just frees the resources from the pool, pool is still here
void vk_cmd_buf_free(
    vk_context* context,
    VkCommandPool pool,
    vk_cmd_buf* cmd_buf
);

void vk_cmd_buf_begin(
    vk_cmd_buf* cmd_buf,
    bool is_single_use,
    bool is_renderpass_continue,
    bool is_simultaneous_use
);

void vk_cmd_buf_end(vk_cmd_buf* cmd_buf);

void vk_cmd_buf_set_submitted(vk_cmd_buf* cmd_buf);

void vk_cmd_buf_reset(vk_cmd_buf* cmd_buf);

/**
 * Allocates and begins recording to out_cmd_buf.
 */
void vk_cmd_buf_allocate_and_begin_single_use(
    vk_context* context,
    VkCommandPool pool,
    vk_cmd_buf* out_cmd_buf
);

/**
 * Ends recording, submits to and waits for queue operation and frees the provided command buffer.
 */
void vk_cmd_buf_end_single_use(
    vk_context* context,
    VkCommandPool pool,
    vk_cmd_buf* cmd_buf,
    VkQueue queue
);