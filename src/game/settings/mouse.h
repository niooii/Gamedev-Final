#pragma once

#include "def.h"

typedef struct MouseSettings {
    f32 x_sensitivity;
    f32 y_sensitivity;
    f32 max_sensitivity;
    f32 min_sensitivity;
    bool inverted;
} MouseSettings;