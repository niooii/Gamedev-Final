#include "settings.h"

static const char* SETTINGS_PATH = "./settings.gdf";
static Settings* LOADED_SETTINGS = NULL;

Settings* GDF_LoadSettings() 
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
