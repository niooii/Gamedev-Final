#include "vk_framebuffer.h"

void vulkan_framebuffer_create(
    vk_context* context,
    vk_renderpass* renderpass,
    u32 width,
    u32 height,
    u32 attachment_count,
    VkImageView* attachments,
    vk_framebuffer* out_framebuffer)
{
    // copy the attachments, renderpass and attachment count
    out_framebuffer->attachments = GDF_Malloc(sizeof(VkImageView) * attachment_count, GDF_MEMTAG_RENDERER);
    for (u32 i = 0; i < attachment_count; i++) 
    {
        out_framebuffer->attachments[i] = attachments[i];
    }
    out_framebuffer->renderpass = renderpass;
    out_framebuffer->attachment_count = attachment_count;

    VkFramebufferCreateInfo framebuffer_create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebuffer_create_info.renderPass = renderpass->handle;
    framebuffer_create_info.attachmentCount = attachment_count;
    framebuffer_create_info.pAttachments = out_framebuffer->attachments;
    framebuffer_create_info.width = width;
    framebuffer_create_info.height = height;
    framebuffer_create_info.layers = 1;

    VK_ASSERT(
        vkCreateFramebuffer(
            context->device.logical,
            &framebuffer_create_info,
            context->allocator,
            &out_framebuffer->handle
        )
    );
}

void vk_framebuffer_destroy(vk_context* context, vk_framebuffer* framebuffer)
{
    vkDestroyFramebuffer(context->device.logical, framebuffer->handle, context->allocator);
    if (framebuffer->attachments) {
        GDF_Free(framebuffer->attachments);
        framebuffer->attachments = NULL;
    }
    framebuffer->handle = NULL;
    framebuffer->renderpass = NULL;
    framebuffer->attachment_count = 0;
}