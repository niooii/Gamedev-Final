#include "client_settings.h"

static const char* SETTINGS_PATH = "./settings.gdf";
static GDF_Settings* LOADED_SETTINGS = NULL;

GDF_Settings* GDF_LoadSettings() 
{
    if (LOADED_SETTINGS != NULL) 
    {
        //TODO! FREE MEM
    }
    //TODO! load settings from file
    return NULL;
}

void GDF_SaveSettings() 
{

}
