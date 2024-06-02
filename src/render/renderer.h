#pragma once

#include "render_types.h"

struct static_mesh_data;
struct platform_state;

bool GDF_InitRenderer(GDF_RENDER_BACKEND_TYPE render_backend_type);
void GDF_DestroyRenderer();

void GDF_Renderer_Resize(u16 width, u16 height);

bool GDF_Renderer_DrawFrame(GDF_RenderPacket* packet);