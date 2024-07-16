#pragma once

#include "render_types.h"
#include "engine/camera.h"

bool GDF_InitRenderer(GDF_RENDER_BACKEND_TYPE render_backend_type);
void GDF_DestroyRenderer();

void GDF_RENDERER_Resize(u16 width, u16 height);

bool GDF_RENDERER_DrawFrame(GDF_RenderPacket* packet);

void GDF_RENDERER_SetCamera(Camera* camera);

bool GDF_RENDERER_LoadTexture(const char* image_path, GDF_Texture* out_texture);