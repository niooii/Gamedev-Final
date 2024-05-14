#include "serde.h"

bool GDF_SerializeMap(GDF_Map* map, char* out_buf)
{
    for (int i = 0; i < GDF_MKEY_NUM_KEYS; i++)
    {
        if (map->entries[i] == NULL)
            continue;
        char line_buff[200];
        char key_buff[150];
        char val_buff[50];
        
        GDF_MKEY_ToString(i, key_buff);
        switch (map->entries[i]->dtype)
        {
            case GDF_MAP_DTYPE_FLOAT:
            {
                sprintf(val_buff, "%f", *(float*)(map->entries[i]->value));
                break;
            }
            case GDF_MAP_DTYPE_INT:
            {
                sprintf(val_buff, "%d", *(int*)(map->entries[i]->value));
                break;
            }
            case GDF_MAP_DTYPE_BOOL:
            {
                sprintf(val_buff, *(bool*)(map->entries[i]->value) ? "true" : "false");
                break;
            }
            case GDF_MAP_DTYPE_STRING:
            {
                sprintf(val_buff, "%s", *(char*)(map->entries[i]->value));
                break;
            }
            case GDF_MAP_DTYPE_MAP:
            {
                LOG_WARN("map nesting not impmented yet");
                break;
            }
        } 

        snprintf(line_buff, 200, "%s=%s\n", key_buff, val_buff);
        strcat(out_buf, line_buff);
    }
}

bool GDF_DeserializeMap(char* data, GDF_Map* out_map)
{

}

bool GDF_WriteMapToFile(GDF_Map* map, const char* rel_path)
{
    char buf[40000];
    GDF_SerializeMap(map, buf);
    GDF_WriteFile(rel_path, buf);
}

bool GDF_ReadMapFromFile(const char* rel_path, GDF_Map* out_map)
{

}