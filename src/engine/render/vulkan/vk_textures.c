#include "vk_textures.h"

#define TEXTURES_FOLDER "resources/textures/"
static const char* __texture_paths[GDF_TEXTURE_INDEX_MAX] = {
    [GDF_TEXTURE_INDEX_DIRT] = TEXTURES_FOLDER "dirt.png",
    [GDF_TEXTURE_INDEX_GRASS_TOP] = TEXTURES_FOLDER "grass_top.png",
    [GDF_TEXTURE_INDEX_GRASS_SIDE] = TEXTURES_FOLDER "grass_side.png",

};

bool vk_block_textures_init(vk_renderer_context* context, vk_block_textures* out_textures)
{
    out_textures->device = &context->device;
    out_textures->allocator = &context->device.allocator;

    VkDescriptorPoolCreateInfo descriptor_pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 1,

    };

    for (u32 i = 0; i < GDF_TEXTURE_INDEX_MAX; i++)
    {
        const char* img_path = __texture_paths[i];
        vk_texture* tex = &out_textures->textures[i];
        tex->image_path = img_path;

        int width, height, channels;
        unsigned char* pixels = stbi_load(img_path, &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels) {
            LOG_ERR("Failed to read image: %s", img_path);
            return false;
        }

        u64 image_size = width * height * 4;
    }

    VkSamplerCreateInfo sampler_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = 16,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .mipLodBias = 0.0f,
        .minLod = 0.0f,
        .maxLod = 0.0f
    };

    VK_RETURN_FALSE_ASSERT(
        vkCreateSampler(
            out_textures->device->handle,
            &sampler_info,
            out_textures->allocator,
            &out_textures->sampler
        )
    );

    return true;
}

void vk_texture_manager_destroy(vk_block_textures* out_texture_manager)
{
    
}
