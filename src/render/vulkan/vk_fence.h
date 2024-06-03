#pragma once
#include "core.h"

#include "vk_types.h"

void vk_fence_create(vk_context* context, bool create_signaled, vk_fence* out_fence);

void vk_fence_destroy(vk_context* context, vk_fence* fence);

bool vk_fence_wait(vk_context* context, vk_fence* fence, u64 timeout_ns);