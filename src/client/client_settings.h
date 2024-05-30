#pragma once

#include "core.h"

typedef struct GDF_ClientSettings {
    bool verbose_output;
    bool client_show_console;
} GDF_ClientSettings;

bool GDF_ClientSettings_Load();
bool GDF_ClientSettings_Save();
GDF_ClientSettings* GDF_AppSettings_Get();