#pragma once

#include "core.h"

// must c hange appsettings serde functions if adding/removing
typedef enum GDF_APPTYPE {
    GDF_APPTYPE_CLIENT,
    GDF_APPTYPE_SERVER
} GDF_APPTYPE;

typedef struct GDF_AppSettings {
    GDF_APPTYPE app_type;
    bool verbose_output;
    bool client_show_console;
} GDF_AppSettings;

bool GDF_AppSettings_Load();
bool GDF_AppSettings_Save();
GDF_AppSettings* GDF_AppSettings_Get();