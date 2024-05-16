#include "map.h"

void GDF_MKEY_ToString(GDF_MKEY key, char* out_str)
{
    switch (key)
    {
        case GDF_MKEY_SETTINGS_DEV_CAN_FLY:
            sprintf(out_str, "SETTINGS_DEV_CAN_FLY");
            break;
        case GDF_MKEY_SETTINGS_DEV_NOCLIP:
            sprintf(out_str, "SETTINGS_DEV_NOCLIP");
            break;
        case GDF_MKEY_SETTINGS_DEV_DRAW_WIREFRAME:
            sprintf(out_str, "SETTINGS_DEV_DRAW_WIREFRAME");
            break;
        default:
            sprintf(out_str, "UNKNOWN");
            break;
    }
}

static t_symstruct lookup_table[GDF_MKEY_NUM_KEYS] = {};
void GDF_MKEY_InitLookupTable()
{
    for (int i = 0; i < GDF_MKEY_NUM_KEYS; i++) {
        char* key_buf = malloc(150);
        GDF_MKEY_ToString(i, key_buf);
        lookup_table[i].key = i;
        lookup_table[i].str = key_buf;
    }
}

GDF_MKEY GDF_MKEY_FromString(const char* str)
{
    for (int i = 0; i < GDF_MKEY_NUM_KEYS; i++) {
        t_symstruct sym = lookup_table[i];
        if (strcmp(sym.str, str) == 0)
            return sym.key;
    }
    return GDF_MKEY_ERROR_KEY;
}

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
            LOG_INFO("this string was sent to map entry: %s", (char*)value);
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
    if (dtype == GDF_MAP_DTYPE_STRING)
    {
        LOG_ERR("STILL GOT THIS THING: %s", (char*)(map->entries[key]->value))
    } 
}