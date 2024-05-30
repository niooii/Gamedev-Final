#pragma once

#include "render_types.h"

struct platform_state;

bool renderer_backend_create(GDF_RENDER_BACKEND_TYPE type, renderer_backend* out_renderer_backend);
void renderer_backend_destroy(renderer_backend* renderer_backend);