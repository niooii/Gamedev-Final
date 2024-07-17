#include "vk_textures.h"
#define STB_IMAGE_IMPLEMENTATION
#include "engine/external/stb_image.h"
#include "engine/core/containers/list.h"
#include "engine/core/os/io.h"
#include "vk_utils.h"
#include "vk_buffers.h"

#define TEXTURES_FOLDER "resources/textures/"
static const char* __texture_paths[GDF_TEXTURE_INDEX_MAX] = {
    [GDF_TEXTURE_INDEX_DIRT] = TEXTURES_FOLDER "dirt.png",
    [GDF_TEXTURE_INDEX_GRASS_TOP] = TEXTURES_FOLDER "grass_top.png",
    [GDF_TEXTURE_INDEX_GRASS_SIDE] = TEXTURES_FOLDER "grass_side.png",

};

bool vk_block_textures_init(vk_renderer_context* context, vk_block_textures* out_textures)
{
    out_textures->device = &context->device;
    vk_device* device = &context->device;
    out_textures->allocator = context->device.allocator;
    VkAllocationCallbacks* allocator = context->device.allocator;

    // Descriptor pool
    VkDescriptorPoolSize pool_sizes[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = context->swapchain.image_count
        }
    };

    VkDescriptorPoolCreateInfo descriptor_pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = context->swapchain.image_count,
        .poolSizeCount = 1,
        .pPoolSizes = pool_sizes
    };

    VK_RETURN_FALSE_ASSERT(
        vkCreateDescriptorPool(
            device->handle,
            &descriptor_pool_info,
            allocator,
            &out_textures->descriptor_pool
        )
    );

    VkDescriptorSetLayoutBinding bindings[] = {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        }
    };

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = bindings
    };

    // Create layouts (each frame in flight)
    out_textures->descriptor_set_layouts = GDF_LIST_Create(VkDescriptorSetLayout);
    for (u32 i = 0; i < context->swapchain.image_count; i++)
    {
        VkDescriptorSetLayout layout;
        VK_RETURN_FALSE_ASSERT(
            vkCreateDescriptorSetLayout(
                out_textures->device->handle, 
                &descriptor_set_layout_info, 
                out_textures->allocator,
                &layout
            )
        );
        GDF_LIST_Push(out_textures->descriptor_set_layouts, layout);
    }

    out_textures->descriptor_sets = GDF_LIST_Reserve(VkDescriptorSet, context->swapchain.image_count);

    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = out_textures->descriptor_pool,
        .descriptorSetCount = GDF_LIST_GetLength(out_textures->descriptor_set_layouts),
        .pSetLayouts = out_textures->descriptor_set_layouts
    };

    VK_RETURN_FALSE_ASSERT(
        vkAllocateDescriptorSets(
            device->handle,
            &alloc_info,
            out_textures->descriptor_sets
        )
    );

    LOG_DEBUG("Allocated descriptor sets for block textures");

    // assume each block texture is 16x16
    const u32 w = 16;
    const u32 h = 16;

    // Create the Image object (array of images)
    VkImageCreateInfo image_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent.width = w,
        .extent.height = h,
        .extent.depth = 1,
        // TODO! add mipmap stuff
        .mipLevels = 1,
        .arrayLayers = GDF_TEXTURE_INDEX_MAX,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VK_RETURN_FALSE_ASSERT(
        vkCreateImage(
            device->handle,
            &image_info,
            allocator,
            &out_textures->texture_array.handle
        )
    );

    VkMemoryRequirements mem_req;
    vkGetImageMemoryRequirements(
        device->handle, 
        out_textures->texture_array.handle, 
        &mem_req
    );

    VkMemoryAllocateInfo img_alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_req.size,
        .memoryTypeIndex = vk_utils_find_memory_type_idx(
            context, 
            mem_req.memoryTypeBits, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        )
    };

    VK_RETURN_FALSE_ASSERT(
        vkAllocateMemory(
            device->handle, 
            &img_alloc_info, 
            allocator, 
            &out_textures->texture_array_memory
        )
    );
    VK_RETURN_FALSE_ASSERT(
        vkBindImageMemory(
            device->handle, 
            out_textures->texture_array.handle, 
            out_textures->texture_array_memory, 
            0
        )
    );

    // Create image view
    VkImageViewCreateInfo view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = out_textures->texture_array.handle,
        .viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = GDF_TEXTURE_INDEX_MAX
        }
    };

    VK_RETURN_FALSE_ASSERT(
        vkCreateImageView(
            device->handle, 
            &view_info, 
            allocator, 
            &out_textures->texture_array.view
        )
    );

    LOG_DEBUG("Created texture image array");

    // Load images for each texture
    // 4 channels
    u64 image_size = w * h * 4;

    vk_buffer staging_buffer;
    VkDeviceSize buffer_size = image_size * GDF_TEXTURE_INDEX_MAX;

    if (
        !vk_buffers_create(
            context, 
            buffer_size, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            &staging_buffer
        )
    )
    {
        LOG_ERR("Failed to create staging buffer (block textures init)..");
        return false;
    }

    void* texture_data;
    VK_RETURN_FALSE_ASSERT(
        vkMapMemory(
            device->handle,
            staging_buffer.memory, 
            0, 
            buffer_size, 
            0, 
            &texture_data
        )
    );

    for (u32 i = 0; i < GDF_TEXTURE_INDEX_MAX; i++)
    {
        const char* img_path = __texture_paths[i];
        char abs_path[900];
        GDF_GetAbsolutePath(img_path, abs_path);
        // vk_texture* tex = &out_textures->textures[i];
        // tex->image_path = img_path;

        int width, height, channels;
        byte* pixels = stbi_load(abs_path, &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels) {
            LOG_ERR("Failed to read image: %s", abs_path);
            return false;
        }
        u32 byte_size = width * height * channels;
        if (byte_size != image_size)
        {
            LOG_ERR(
                "Expected block texture to be %d bytes, got %d instead (%s)",
                image_size,
                byte_size,
                img_path
            );
            stbi_image_free(pixels);
            return false;
        }

        GDF_MemCopy((byte*)texture_data + i * image_size, pixels, image_size);
        LOG_DEBUG("Loaded texture id %u into staging buffer.", i);
        stbi_image_free(pixels);
    }

    vkUnmapMemory(
        device->handle,
        staging_buffer.memory
    );

    // TODO! copy staging buffer into image. allocate new command buffer

    VkBufferImageCopy copy_region = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .imageSubresource.mipLevel = 0,
        .imageSubresource.baseArrayLayer = 0,
        .imageSubresource.layerCount = GDF_TEXTURE_INDEX_MAX,
        .imageOffset = {0, 0, 0},
        .imageExtent = {w, h, 1}
    };

    // vkCmdCopyBufferToImage(

    // );

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
            device->handle,
            &sampler_info,
            allocator,
            &out_textures->sampler
        )
    );

    return true;
}

void vk_texture_manager_destroy(vk_block_textures* out_texture_manager)
{
    
}
