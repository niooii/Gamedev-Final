#include "map.h"

GDF_Map* GDF_CreateMap()
{
    GDF_Map* map = malloc(sizeof(GDF_Map));
    for (int i = 0; i < GDF_MKEY_NUM_KEYS; i++)
    {
        map->entries[i] = NULL;
    }
    return map;
}

bool GDF_AddMapEntry(GDF_Map* map, GDF_MKEY key, void* value, GDF_MAP_DTYPE dtype)
{
    if (key == GDF_MKEY_ERROR_KEY)
    {
        LOG_ERR("Tried to add ERROR_KEY to map, something went wrong...");
        return false;
    }
    if (map->entries[key] != NULL)
    {
        LOG_WARN("already key here retard");
        return false;
    }
    GDF_MapEntry* entry = malloc(sizeof(GDF_MapEntry));
    entry->dtype = dtype;
    // get value size for memcpy
    size_t value_size;
    switch (dtype)
    {
        case GDF_MAP_DTYPE_DOUBLE:
        {
            value_size = sizeof(f64);
            break;
        }
        case GDF_MAP_DTYPE_INT:
        {
            value_size = sizeof(i32);
            break;
        }
        case GDF_MAP_DTYPE_BOOL:
        {
            value_size = sizeof(bool);
            break;
        }
        case GDF_MAP_DTYPE_STRING:
        {
            value_size = strlen((char*)value);
            break;
        }
        case GDF_MAP_DTYPE_MAP:
        {
            value_size = (int)("womp womp");
            LOG_WARN("map nesting not impmented yet");
            break;
        }
    } 
    void* value_clone = malloc(value_size);
    if (dtype == GDF_MAP_DTYPE_STRING)
    {
        strcpy(value_clone, (char*)value);
    }
    else
    {
        memcpy(value_clone, value, value_size);
    }
    entry->value = value_clone;
    map->entries[key] = entry;   
    return true;
}

void GDF_FreeMap(GDF_Map* map)
{
    GDF_Map* map = malloc(sizeof(GDF_Map));
    for (int i = 0; i < GDF_MKEY_NUM_KEYS; i++)
    {
        if (map->entries[i] != NULL)
        {
            free(map->entries[i]->value);
            free(map->entries[i]);
        }
    }
    free(map);
}