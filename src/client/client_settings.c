#include "client/client_settings.h"
#include "core/serde/serde.h"
static GDF_ClientSettings settings;

bool GDF_ClientSettings_Load()
{
    GDF_Map* map = GDF_CreateMap();
    GDF_ReadMapFromFile("client_settings.gdf", map);
    void* val = GDF_MAP_GetValueBool(map, GDF_MKEY_CLIENT_SETTINGS_VERBOSE_OUTPUT);
    if (val == NULL)
        return false;
    settings.verbose_output = *(bool*)val;
    val = GDF_MAP_GetValueBool(map, GDF_MKEY_CLIENT_SETTINGS_SHOW_CONSOLE);
    if (val == NULL)
        return false;
    settings.client_show_console = *(bool*)val;
    GDF_FreeMap(map);
    return true;
}

bool GDF_ClientSettings_Save()
{
    GDF_Map* map = GDF_CreateMap();
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_CLIENT_SETTINGS_VERBOSE_OUTPUT, 
        &settings.verbose_output, 
        GDF_MAP_DTYPE_BOOL
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_CLIENT_SETTINGS_SHOW_CONSOLE, 
        &settings.client_show_console, 
        GDF_MAP_DTYPE_BOOL
    );
    GDF_WriteMapToFile(map, "client_settings.gdf");
    GDF_FreeMap(map);

    return true;  
}

// returns a static instance of appsettings because it will be used everywhere.
GDF_ClientSettings* GDF_AppSettings_Get()
{
    return &settings;
}