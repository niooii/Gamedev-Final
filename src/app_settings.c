#include "app_settings.h"
#include "core/serde/serde.h"
static GDF_AppSettings settings;

bool GDF_AppSettings_Load()
{
    GDF_Map* map = GDF_CreateMap();
    GDF_ReadMapFromFile("app_settings.gdf", map);
    settings.verbose_output = *GDF_MAP_GetValueBool(map, GDF_MKEY_APP_SETTINGS_VERBOSE_OUTPUT);
    char* buf = GDF_MAP_GetValueString(map, GDF_MKEY_APP_SETTINGS_APP_TYPE);
    if (strcmp("client", buf) == 0)
    {
        settings.app_type = GDF_APPTYPE_CLIENT;
    }
    else if (strcmp("server", buf) == 0)
    {
        settings.app_type = GDF_APPTYPE_SERVER;
    }
    else
    {
        LOG_WARN("Unknown option for app_type: \"%s\". Defaulting to client.", buf);
        settings.app_type = GDF_APPTYPE_CLIENT;
    }
    GDF_FreeMap(map);
    return true;
}

bool GDF_AppSettings_Save()
{
    GDF_Map* map = GDF_CreateMap();
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_APP_SETTINGS_APP_TYPE, 
        settings.app_type == GDF_APPTYPE_CLIENT ? "client" : "server", 
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_APP_SETTINGS_VERBOSE_OUTPUT, 
        &settings.verbose_output, 
        GDF_MAP_DTYPE_BOOL
    );
    GDF_WriteMapToFile(map, "app_settings.gdf");
    GDF_FreeMap(map);
}

// returns a static instance of appsettings because it will be used everywhere.
GDF_AppSettings* GDF_AppSettings_Get()
{
    return &settings;
}