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
    u32 line_num = 0;
    while(line) {
        line_num++;
        memset(line_buf, 0, 200);
        memset(key_buf, 0, 150);
        memset(val_buf, 0, 50);
        sscanf(line, "%[^=]=%s", key_buf, val_buf);
        
        if (GDF_AppSettings_Get()->verbose_output)
        {
            LOG_DEBUG("key: %s", key_buf);
            LOG_DEBUG("value: %s", val_buf);
        }

        // TODO! get dtype, convert to said dtype then to 
        // void* and memcpy soemthing something
        GDF_MAP_DTYPE dtype;
        void* value = NULL;
        bool string_reads_true;
        if (value == NULL && strncmp(val_buf, "\"", 1) == 0 && strncmp(val_buf + strlen(val_buf) - 1, "\"", 1) == 0)
        {
            if (GDF_AppSettings_Get()->verbose_output)
                LOG_INFO("found string")
            // string value
            dtype = GDF_MAP_DTYPE_STRING;
            size_t len = strlen(val_buf);
            value = malloc(sizeof(char) * len - 1);
            
            strncpy(value, val_buf + 1, len - 2);
            ((char*)value)[len - 2] = '\0';
        }
        if (value == NULL && 
        ((string_reads_true = strcmp(val_buf, "true") == 0) || strcmp(val_buf, "false") == 0))
        {
            if (GDF_AppSettings_Get()->verbose_output)
                LOG_DEBUG("dtype: bool")
            // then is bool value
            dtype = GDF_MAP_DTYPE_BOOL;
            value = malloc(sizeof(bool));
            *((bool*)value) = string_reads_true ? true : false;
        }
        if (value == NULL && strncmp(val_buf, "[", 1))
        {
            LOG_INFO("value is a map type, deserialization not implemented");
        }
        // TODO! double and int 
        if (value == NULL && strchr(val_buf, '.') != NULL)
        {
            // could be a double
            f64 f;
            if (sscanf(val_buf, "%lf", &f) != 0)
            {
                if (GDF_AppSettings_Get()->verbose_output)
                    LOG_DEBUG("dtype: double");
                dtype = GDF_MAP_DTYPE_DOUBLE;
                value = malloc(sizeof(f64));
                *((f64*)value) = f;
            }
        }
        if (value == NULL)
        {
            // could be an int, if not its some unknown thing
            i32 i;
            if (sscanf(val_buf, "%d", &i) == 0)
            {
                LOG_ERR("found bad value at line %d, stopping map deserialization...", line_num);
                return false;
            }
            if (GDF_AppSettings_Get()->verbose_output)
                LOG_DEBUG("dtype: int");
            dtype = GDF_MAP_DTYPE_INT;
            value = malloc(sizeof(i32));
            *((i32*)value) = i;
        }
        GDF_AddMapEntry(out_map, GDF_MKEY_FromString(key_buf), value, dtype);
        free(value);
        line = strtok(NULL, "\n");
    }
    return true;
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