#pragma once

#include "render_types.h"
#include "engine/camera.h"

bool GDF_InitRenderer(GDF_RENDER_BACKEND_TYPE render_backend_type);
void GDF_DestroyRenderer();

void renderer_resize(u16 width, u16 height);

bool renderer_draw_frame(GDF_RenderPacket* packet);

void renderer_set_camera(GDF_Camera* camera);

Renderer* renderer_get_instance(); 

// bool GDF_RENDERER_LoadTexture(const char* image_path, GDF_Texture* out_texture);