#include "vk_renderer.h"
#include "vk_types.h"
#include "core/containers/list.h"
#include "vk_os.h"

static vk_context context;
static u32 cached_framebuf_w;
static u32 cached_framebuf_h;

VKAPI_ATTR VkBool32 VKAPI_CALL __vk_dbg_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
    switch (message_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_TRACE(callback_data->pMessage);
            break;
    }
    return VK_FALSE;
}

// filters the context.physical_device_info list 
static void __filter_available_devices()
{
    u32 list_len = GDF_LIST_GetLength(context.physical_device_info_list);
    for (u32 i = list_len; i > 0; i--)
    {
        vk_physical_device* device = &context.physical_device_info_list[i - 1]; 
        LOG_DEBUG("Checking device %s", device->properties.deviceName);

        bool device_suitable = true;
        
        // check swapchain support
        if (device->sc_support_info.format_count < 1 || device->sc_support_info.present_mode_count < 1)
        {
            device_suitable = false;
            goto filter_device_skip;
        }
        // check queues support
        if (
            device->queues.compute_family_index == -1 ||
            device->queues.graphics_family_index == -1 ||
            device->queues.present_family_index == -1 ||
            device->queues.transfer_family_index == -1
        )
        {
            LOG_DEBUG("Device does not have the required queues.");
            device_suitable = false;
            goto filter_device_skip;
        }    
        // TODO! Check if the device supports
        // all the required extensions

        filter_device_skip:
        if (!device_suitable)
        {
            LOG_DEBUG("Marked device \'%s\' as unusable.", device->properties.deviceName);
            device->usable = false;
        }
    }
}

void __create_cmd_bufs(renderer_backend* backend)
{
    if (context.graphics_cmd_buf_list == NULL) 
    {
        context.graphics_cmd_buf_list = GDF_LIST_Reserve(vk_cmd_buf, context.swapchain.image_count);
        for (u32 i = 0; i < context.swapchain.image_count; i++) 
        {
            GDF_MemZero(&context.graphics_cmd_buf_list[i], sizeof(vk_cmd_buf));
        }
    }

    for (u32 i = 0; i < context.swapchain.image_count; ++i) 
    {
        if (context.graphics_cmd_buf_list[i].handle) {
            vk_cmd_buf_free(
                &context,
                context.device.graphics_cmd_pool,
                &context.graphics_cmd_buf_list[i]
            );
        }
        GDF_MemZero(&context.graphics_cmd_buf_list[i], sizeof(vk_cmd_buf));
        vk_cmd_buf_allocate(
            &context,
            context.device.graphics_cmd_pool,
            true,
            &context.graphics_cmd_buf_list[i]
        );
    }

    LOG_DEBUG("Vulkan command buffers created.");
}

void __regenerate_framebuffers(renderer_backend* backend, vk_swapchain* swapchain, vk_renderpass* renderpass) {
    for (u32 i = 0; i < swapchain->image_count; i++) 
    {
        // TODO! make this dynamic based on the currently configured attachments
        u32 attachment_count = 2;
        VkImageView attachments[] = {
            swapchain->views[i],
            swapchain->depth_attachment.view};

        vulkan_framebuffer_create(
            &context,
            renderpass,
            context.framebuffer_width,
            context.framebuffer_height,
            attachment_count,
            attachments,
            &context.swapchain.framebuffers[i]);
    }
}

bool __recreate_swapchain(renderer_backend* backend) {
    if (context.recreating_swapchain) {
        LOG_WARN("Already recreating swapchain calm down there");
        return false;
    }

    // if the window is 0 w or h dont do anything
    if (cached_framebuf_w == 0 || cached_framebuf_h == 0) {
        return false;
    }

    context.recreating_swapchain = true;

    // in case theres any operations going on rn
    vkDeviceWaitIdle(context.device.logical);

    // clear these out just in case.
    for (u32 i = 0; i < context.swapchain.image_count; i++) 
    {
        context.images_in_flight[i] = NULL;
    }

    // requery details 
    vk_device_query_swapchain_support(
        context.device.physical_info->handle,
        context.surface,
        &context.device.physical_info->sc_support_info
    );
    vk_device_find_depth_format(&context.device);

    vk_swapchain_recreate(
        &context,
        cached_framebuf_w,
        cached_framebuf_h,
        &context.swapchain
    );

    // update the sizes with cached values
    context.framebuffer_width = cached_framebuf_w;
    context.framebuffer_height = cached_framebuf_h;
    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;
    cached_framebuf_w = 0;
    cached_framebuf_h = 0;

    // cleanup swapchain
    for (u32 i = 0; i < context.swapchain.image_count; i++) 
    {
        vk_cmd_buf_free(&context, context.device.graphics_cmd_pool, &context.graphics_cmd_buf_list[i]);
    }

    // framebuffers
    for (u32 i = 0; i < context.swapchain.image_count; i++) 
    {
        vk_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
    }

    context.main_renderpass.x = 0;
    context.main_renderpass.y = 0;
    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;

    __regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);

    __create_cmd_bufs(backend);

    // done recreating
    context.recreating_swapchain = false;

    return true;
}

// ===== FORWARD DECLARATIONS END =====
bool vk_renderer_init(renderer_backend* backend, const char* application_name) 
{
    // TODO! custom allocator.
    context.allocator = 0;
    
    GDF_GetWindowSize(&cached_framebuf_w, &cached_framebuf_h);
    context.framebuffer_width = cached_framebuf_w;
    context.framebuffer_height = cached_framebuf_h;
    LOG_INFO("Framebuffer size %d, %d", context.framebuffer_width, context.framebuffer_height);
    cached_framebuf_w = 0;
    cached_framebuf_h = 0;


    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "GDF";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; 
    create_info.pApplicationInfo = &app_info;

    // extensions
    const char** extensions = GDF_LIST_Create(const char*);
    GDF_LIST_Push(extensions, &VK_KHR_SURFACE_EXTENSION_NAME);  // Generic surface extension
    GDF_VK_GetRequiredExtensionNames(&extensions);

#ifndef GDF_RELEASE
    GDF_LIST_Push(extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    create_info.enabledExtensionCount = GDF_LIST_GetLength(extensions);
    create_info.ppEnabledExtensionNames = extensions;

    // validation layers
#ifndef GDF_RELEASE
    const char** validation_layers = GDF_LIST_Create(const char*);
    GDF_LIST_Push(validation_layers, &"VK_LAYER_KHRONOS_validation");
    create_info.enabledLayerCount = GDF_LIST_GetLength(validation_layers);
    create_info.ppEnabledLayerNames = validation_layers;
#endif
    VK_ASSERT(vkCreateInstance(&create_info, context.allocator, &context.instance));

    LOG_DEBUG("Vulkan instance initialized successfully.");

#ifndef GDF_RELEASE
    LOG_DEBUG("Creating Vulkan debugger...");
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;  //|
                                                                      //    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = __vk_dbg_callback;

    PFN_vkCreateDebugUtilsMessengerEXT f =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
    GDF_ASSERT_MSG((f) != NULL, "Function returned was NULL.");
    VK_ASSERT(f(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));
    LOG_DEBUG("Vulkan debugger created.");
#endif
    
    // enumerate physical devices then create logical 
    u32 physical_device_count = 0;
    VK_ASSERT(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, NULL));
    if (physical_device_count == 0)
    {
        LOG_FATAL("There are no devices supporting vulkan on your system.");
        return false;
    }
    VkPhysicalDevice physical_devices[physical_device_count];
    VK_ASSERT(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, physical_devices));

    // test surface creation remove later and place elsewhwere maybe
    GDF_VK_CreateSurface(&context);

    context.physical_device_info_list = GDF_LIST_Create(vk_physical_device);
    for (u32 i = 0; i < physical_device_count; i++)
    {
        vk_physical_device info = {
            .handle = physical_devices[i],
            .usable = true
        };
        // fill properties, features, and memoryinfo field
        vkGetPhysicalDeviceProperties(physical_devices[i], &info.properties);
        vkGetPhysicalDeviceFeatures(physical_devices[i], &info.features);
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &info.memory);
        // fill the swapchain support info field
        vk_device_query_swapchain_support(info.handle, context.surface, &info.sc_support_info);
        // fill the queue info field
        vk_device_query_queues(&context, info.handle, &info.queues);
        // TODO! may be a memory bug here check it out later
        GDF_LIST_Push(context.physical_device_info_list, info);
    }

    __filter_available_devices();
    vk_device_create(&context, NULL);

    vk_swapchain_create(
        &context,
        context.framebuffer_width,
        context.framebuffer_height,
        &context.swapchain
    );

    vk_renderpass_create(
        &context,
        &context.main_renderpass,
        0, 0, context.framebuffer_width, context.framebuffer_height,
        0.0f, 0.0f, 0.2f, 1.0f,
        1.0f,
        0
    );

    context.swapchain.framebuffers = GDF_LIST_Reserve(vk_framebuffer, context.swapchain.image_count);
    __regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);

    __create_cmd_bufs(backend);

    context.image_available_semaphores = GDF_LIST_Reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
    context.queue_complete_semaphores = GDF_LIST_Reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
    context.in_flight_fences = GDF_LIST_Reserve(vk_fence, context.swapchain.max_frames_in_flight);

    for (u8 i = 0; i < context.swapchain.max_frames_in_flight; i++) 
    {
        VkSemaphoreCreateInfo semaphore_create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        vkCreateSemaphore(context.device.logical, &semaphore_create_info, context.allocator, &context.image_available_semaphores[i]);
        vkCreateSemaphore(context.device.logical, &semaphore_create_info, context.allocator, &context.queue_complete_semaphores[i]);

        // Create the fence in a signaled state, indicating that the first frame has already been "rendered".
        // This will prevent the application from waiting indefinitely for the first frame to render since it
        // cannot be rendered until a frame is "rendered" before it.
        vk_fence_create(&context, true, &context.in_flight_fences[i]);
    }

    // In flight fences should not yet exist at this point, so clear the list. These are stored in pointers
    // because the initial state should be 0, and will be 0 when not in use. Acutal fences are not owned
    // by this list.
    context.images_in_flight = GDF_LIST_Reserve(vk_fence, context.swapchain.image_count);
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        context.images_in_flight[i] = NULL;
    }

    LOG_INFO("Finished initialization of vulkan stuff...");

    return true;
}

void vk_renderer_destroy(renderer_backend* backend) 
{
    vkDeviceWaitIdle(context.device.logical);
    // destroy in the opposite order of creation.

    // sync objects
    for (u8 i = 0; i < context.swapchain.max_frames_in_flight; i++) 
    {
        if (context.image_available_semaphores[i]) 
        {
            vkDestroySemaphore(
                context.device.logical,
                context.image_available_semaphores[i],
                context.allocator);
            context.image_available_semaphores[i] = NULL;
        }
        if (context.queue_complete_semaphores[i]) 
        {
            vkDestroySemaphore(
                context.device.logical,
                context.queue_complete_semaphores[i],
                context.allocator);
            context.queue_complete_semaphores[i] = NULL;
        }
        vk_fence_destroy(&context, &context.in_flight_fences[i]);
    }
    GDF_LIST_Destroy(context.image_available_semaphores);
    context.image_available_semaphores = NULL;

    GDF_LIST_Destroy(context.queue_complete_semaphores);
    context.queue_complete_semaphores = NULL;

    GDF_LIST_Destroy(context.in_flight_fences);
    context.in_flight_fences = NULL;

    GDF_LIST_Destroy(context.images_in_flight);
    context.images_in_flight = NULL;

    // command bufs
    for (u32 i = 0; i < context.swapchain.image_count; i++) 
    {
        if (context.graphics_cmd_buf_list[i].handle) 
        {
            vk_cmd_buf_free(
                &context,
                context.device.graphics_cmd_pool,
                &context.graphics_cmd_buf_list[i]
            );
            context.graphics_cmd_buf_list[i].handle = NULL;
        }
    }
    GDF_LIST_Destroy(context.graphics_cmd_buf_list);
    context.graphics_cmd_buf_list = NULL;

    // framebuffers
    for (u32 i = 0; i < context.swapchain.image_count; i++) 
    {
        vk_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
    }

    // renderpass
    vk_renderpass_destroy(&context, &context.main_renderpass);

    // swapchain
    vk_swapchain_destroy(&context, &context.swapchain);

    // device
    LOG_DEBUG("Destroying Vulkan device...");
    vk_device_destroy(&context);

    LOG_DEBUG("Destroying Vulkan surface...");
    if (context.surface) {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = 0;
    }

    LOG_DEBUG("Destroying Vulkan debugger...");
#ifndef GDF_RELEASE
    if (context.debug_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT f =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
        f(context.instance, context.debug_messenger, context.allocator);
    }
#endif
    LOG_DEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(context.instance, context.allocator);
}

void vk_renderer_resize(renderer_backend* backend, u16 width, u16 height) 
{
    cached_framebuf_w = width;
    cached_framebuf_h = height;
    context.pending_resize_event = true;
}

bool vk_renderer_begin_frame(renderer_backend* backend, f32 delta_time) 
{
    vk_device* device = &context.device;

    // Check if recreating swap chain and boot out.
    if (context.recreating_swapchain) {
        if (!vk_result_is_success(vkDeviceWaitIdle(device->logical))) {
            LOG_ERR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (1) failed");
            return false;
        }
        LOG_DEBUG("Swapchain is being recreated rn..");
        return false;
    }

    // Check if the framebuffer has been resized. If so, a new swapchain must be created.
    if (context.pending_resize_event) 
    {
        if (!vk_result_is_success(vkDeviceWaitIdle(device->logical)))
        {
            LOG_ERR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (2) failed");
            return false;
        }

        // If the swapchain recreation failed (because, for example, the window was minimized),
        // boot out before unsetting the flag.
        if (!__recreate_swapchain(backend)) {
            return false;
        }

        LOG_INFO("Resized successfully.");
        context.pending_resize_event = false;
        return false;
    }

    // Wait for the execution of the current frame to complete. The fence being free will allow this one to move on.
    if (
        !vk_fence_wait(
            &context,
            &context.in_flight_fences[context.current_frame],
            UINT64_MAX
            )
        ) 
    {
        LOG_WARN("In-flight fence wait failure!");
        return false;
    }

    // Acquire the next image from the swap chain. Pass along the semaphore that should signaled when this completes.
    // This same semaphore will later be waited on by the queue submission to ensure this image is available.
    if (!vk_swapchain_acquire_next_image_index(
            &context,
            &context.swapchain,
            UINT64_MAX,
            context.image_available_semaphores[context.current_frame],
            0,
            &context.img_idx)) {
        return false;
    }

    // Begin recording commands.
    vk_cmd_buf* command_buffer = &context.graphics_cmd_buf_list[context.img_idx];
    vk_cmd_buf_reset(command_buffer);
    vk_cmd_buf_begin(command_buffer, false, false, false);

    // make as similar to opengls as possible
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = (f32)context.framebuffer_height;
    viewport.width = (f32)context.framebuffer_width;
    viewport.height = -(f32)context.framebuffer_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // scissor
    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context.framebuffer_width;
    scissor.extent.height = context.framebuffer_height;

    vkCmdSetViewport(command_buffer->handle, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer->handle, 0, 1, &scissor);

    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;

    vk_renderpass_begin(
        command_buffer,
        &context.main_renderpass,
        context.swapchain.framebuffers[context.img_idx].handle
    );

    return true;
}

bool vk_renderer_end_frame(renderer_backend* backend, f32 delta_time) 
{
    vk_cmd_buf* command_buffer = &context.graphics_cmd_buf_list[context.img_idx];

    vk_renderpass_end(command_buffer, &context.main_renderpass);

    // end recording
    vk_cmd_buf_end(command_buffer);

    // Make sure the previous frame is not using this image (i.e. its fence is being waited on)
    if (context.images_in_flight[context.img_idx] != VK_NULL_HANDLE) {  // was frame
        vk_fence_wait(
            &context,
            context.images_in_flight[context.img_idx],
            UINT64_MAX
        );
    }

    // Mark the image fence as in-use by this frame.
    context.images_in_flight[context.img_idx] = &context.in_flight_fences[context.current_frame];

    // Reset the fence for use on the next frame
    vk_fence_reset(&context, &context.in_flight_fences[context.current_frame]);

    // Submit the queue and wait for the operation to complete.
    // Begin queue submission
    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};

    // Command buffer(s) to be executed.
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->handle;

    // The semaphore(s) to be signaled when the queue is complete.
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &context.queue_complete_semaphores[context.current_frame];

    // Wait semaphore ensures that the operation cannot begin until the image is available.
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &context.image_available_semaphores[context.current_frame];

    // Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
    // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent colour attachment
    // writes from executing until the semaphore signals (i.e. one frame is presented at a time)
    VkPipelineStageFlags flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.pWaitDstStageMask = flags;

    VK_ASSERT(
        vkQueueSubmit(
            context.device.graphics_queue,
            1,
            &submit_info,
            context.in_flight_fences[context.current_frame].handle
        )
    );

    vk_cmd_buf_set_submitted(command_buffer);
    // End queue submission

    // Give the image back to the swapchain.
    vk_swapchain_present(
        &context,
        &context.swapchain,
        context.device.graphics_queue,
        context.device.present_queue,
        context.queue_complete_semaphores[context.current_frame],
        context.img_idx
    );


    return true;
}
