#pragma once

#include "render_types.h"

bool renderer_backend_create(GDF_RENDER_BACKEND_TYPE type, Renderer* out_renderer_backend);
void renderer_backend_destroy(Renderer* renderer_backend);