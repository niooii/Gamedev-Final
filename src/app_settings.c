#include "app_settings.h"
#include "core/serde/serde.h"
bool GDF_AppSettings_Load(GDF_AppSettings* settings)
{
    GDF_Map* map = GDF_CreateMap();
    GDF_ReadMapFromFile("app_settings.gdf", map);
    // TODO! GDF_GetMapEntry
    GDF_FreeMap(map);
}

bool GDF_AppSettings_Save(GDF_AppSettings* settings)
{
    GDF_Map* map = GDF_CreateMap();
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_APP_SETTINGS_APP_TYPE, 
        settings->app_type == GDF_APPTYPE_CLIENT ? "client" : "server", 
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_APP_SETTINGS_VERBOSE_OUTPUT, 
        settings->verbose_output, 
        GDF_MAP_DTYPE_BOOL
    );
    GDF_WriteMapToFile(map, "app_settings.gdf");
    GDF_FreeMap(map);
}