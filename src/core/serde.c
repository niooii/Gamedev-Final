#include "serde.h"

bool GDF_SerializeMap(GDF_Map* map, char* out_buf)
{
    for (int i = 0; i < GDF_MKEY_NUM_KEYS; i++)
    {
        if (map->entries[i] == NULL)
            continue;
        char line_buf[200];
        char key_buf[150];
        char val_buf[50];
        
        GDF_MKEY_ToString(i, key_buf);
        switch (map->entries[i]->dtype)
        {
            case GDF_MAP_DTYPE_DOUBLE:
            {
                sprintf(val_buf, "%lf", *(f64*)(map->entries[i]->value));
                break;
            }
            case GDF_MAP_DTYPE_INT:
            {
                sprintf(val_buf, "%d", *(i32*)(map->entries[i]->value));
                break;
            }
            case GDF_MAP_DTYPE_BOOL:
            {
                sprintf(val_buf, *(bool*)(map->entries[i]->value) ? "true" : "false");
                break;
            }
            case GDF_MAP_DTYPE_STRING:
            {
                sprintf(val_buf, "\"%s\"", (char*)(map->entries[i]->value));
                break;
            }
            case GDF_MAP_DTYPE_MAP:
            {
                LOG_WARN("map nesting not impmented yet");
                break;
            }
        } 

        snprintf(line_buf, 200, "%s=%s\n", key_buf, val_buf);
        strcat(out_buf, line_buf);
    }
}

bool GDF_DeserializeToMap(char* data, GDF_Map* out_map)
{
    char line_buf[200];    
    char key_buf[150];
    char val_buf[50];

    // iterate through lines
    char* line = strtok(strdup(data), "\n");
    while(line) {
        memset(line_buf, 0, 200);
        memset(key_buf, 0, 150);
        memset(val_buf, 0, 50);
        sscanf(line, "%[^=]=%s", key_buf, val_buf);
        LOG_DEBUG("key: %s", key_buf);
        LOG_DEBUG("value: %s", val_buf);

        // TODO! get dtype, convert to said dtype then to 
        // void* and memcpy soemthing something
        GDF_MAP_DTYPE dtype;
        void* value;
        bool string_reads_true;
        if (strncmp(val_buf, "\"", 1) == 0)
        {
            // string value
            LOG_INFO("read a string from gdfdtm");
            size_t len = strlen(val_buf);
            value = malloc(sizeof(char) * len);
            // TODO! null terminated bullshit and
            // separate inside string from quotes
            strcpy(value, val_buf);
        }
        else if ((string_reads_true = strcmp(val_buf, "true") == 0) || strcmp(val_buf, "false"))
        {
            // then is bool value
            dtype = GDF_MAP_DTYPE_BOOL;
            value = malloc(sizeof(bool));
            *((bool*)value) = string_reads_true ? true : false;
        }
        // TODO! double int and map 
        GDF_AddMapEntry(out_map, GDF_MKEY_FromString(key_buf), value, dtype);
        line = strtok(NULL, "\n");
    }
    return 0;
}
// max write capacity of 1mb
bool GDF_WriteMapToFile(GDF_Map* map, const char* rel_path)
{
    char* buf = malloc(sizeof(char) * MB_BYTES);
    GDF_SerializeMap(map, buf);
    GDF_WriteFile(rel_path, buf);
    free(buf);
}
// max read capacity of 1mb
bool GDF_ReadMapFromFile(const char* rel_path, GDF_Map* out_map)
{
    char* buf = malloc(sizeof(char) * MB_BYTES);
    GDF_ReadFile(rel_path, buf, MB_BYTES);
    GDF_DeserializeToMap(buf, out_map);
    free(buf);
}