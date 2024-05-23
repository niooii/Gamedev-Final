#include "map.h"

GDF_Map* GDF_CreateMap()
{
    GDF_Map* map = GDF_Malloc(sizeof(GDF_Map), GDF_MEMTAG_TEMP_RESOURCE);
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
        LOG_WARN("already key here");
        return false;
    }
    GDF_MapEntry* entry = GDF_Malloc(sizeof(GDF_MapEntry), GDF_MEMTAG_TEMP_RESOURCE);
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
    void* value_clone = GDF_Malloc(value_size, GDF_MEMTAG_TEMP_RESOURCE);
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

GDF_MapEntry* GDF_GetMapEntry(GDF_Map* map, GDF_MKEY key)
{
    return map->entries[key];
}

// utility
static void* get_value_checked(GDF_Map* map, GDF_MKEY key, GDF_MAP_DTYPE dtype)
{
    GDF_MapEntry* entry = map->entries[key];
    if (entry == NULL)
    {
        char buf[100];
        GDF_MKEY_ToString(key, buf);
        LOG_WARN("Specified map key does not have an entry: %s", buf);
        return NULL;
    }
    if (entry->dtype != dtype)
    {
        char buf[100];
        GDF_MKEY_ToString(key, buf);
        LOG_WARN("Specified map key's entry has the wrong dtype: %s", buf);
        return NULL;
    }
    return entry->value;
} 

bool* GDF_MAP_GetValueBool(GDF_Map* map, GDF_MKEY key)
{
    void* val = get_value_checked(map, key, GDF_MAP_DTYPE_BOOL);
    if (val != NULL)
        return (bool*)val;
    return NULL;
}
f64* GDF_MAP_GetValuef64(GDF_Map* map, GDF_MKEY key)
{
    void* val = get_value_checked(map, key, GDF_MAP_DTYPE_DOUBLE);
    if (val != NULL)
        return (f64*)val;
    return NULL;
}
i32* GDF_MAP_GetValuei32(GDF_Map* map, GDF_MKEY key)
{
    void* val = get_value_checked(map, key, GDF_MAP_DTYPE_INT);
    if (val != NULL)
        return (i32*)val;
    return NULL;
}
const char* GDF_MAP_GetValueString(GDF_Map* map, GDF_MKEY key)
{
    void* val = get_value_checked(map, key, GDF_MAP_DTYPE_STRING);
    if (val != NULL)
        return (char*)val;
    
    return NULL;
}
GDF_Map* GDF_MAP_GetValueMap(GDF_Map* map, GDF_MKEY key)
{
    void* val = get_value_checked(map, key, GDF_MAP_DTYPE_MAP);
    if (val != NULL)
        return (GDF_Map*)val;
    return NULL;
}

void GDF_FreeMap(GDF_Map* map)
{
    for (int i = 0; i < GDF_MKEY_NUM_KEYS; i++)
    {
        if (map->entries[i] != NULL)
        {
            GDF_Free(map->entries[i]->value);
            GDF_Free(map->entries[i]);
        }
    }
    GDF_Free(map);
}