#include "serde.h"

// Function to replace %ENV_VAR% with the value of the environment variable
static void replace_env_vars(const char* input, char* out_buf) {
    // Allocate an initial buffer for the resulting string
    size_t buffer_size = strlen(input) + 1;
    char* result = malloc(buffer_size);
    if (!result) {
        out_buf = NULL;
        return;  // Failed to allocate memory
    }
    strcpy(result, input);

    char* start;
    while ((start = strchr(result, '%')) != NULL) {
        char* end = strchr(start + 1, '%');
        if (!end) break;  // No closing % found, stop processing

        size_t var_len = end - start - 1;
        char var_name[var_len + 1];
        strncpy(var_name, start + 1, var_len);
        var_name[var_len] = '\0';

        // Get the environment variable value
        const char* var_value = getenv(var_name);
        if (!var_value) var_value = "";  // If variable not found, use empty string

        // Calculate the new buffer size and allocate new buffer
        size_t result_len = strlen(result);
        size_t var_value_len = strlen(var_value);
        size_t new_size = result_len - var_len - 2 + var_value_len + 1;
        char* new_result = malloc(new_size);
        if (!new_result) {
            free(result);
            out_buf = NULL;
            return;  // Failed to allocate memory
        }

        // Create the new result string
        strncpy(new_result, result, start - result);
        strcpy(new_result + (start - result), var_value);
        strcpy(new_result + (start - result) + var_value_len, end + 1);

        free(result);
        strcpy(out_buf, new_result);
        result = new_result;
    }
}

// TODO!: the serialization of a map entry that
// has loaded an environment variable 
// should output the environment variable
// in proper form: key=%env_name% when serialized
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

    LOG_DEBUG("Serialized map: \n%s", out_buf);
    return true;
}

bool GDF_DeserializeToMap(char* data, GDF_Map* out_map)
{
    char line_buf[650];    
    char key_buf[150];
    char val_buf[500];

    // iterate through lines
    char* line = strtok(data, "\n");
    u32 line_num = 0;
    while(line) {
        line_num++;
        memset(line_buf, 0, 650);
        memset(key_buf, 0, 150);
        memset(val_buf, 0, 500);
        sscanf(line, "%[^=]=%[^\n]", key_buf, val_buf);

        char* loc_of_percent = strchr(val_buf, '%');
        // escape character will be backslash or something
        // but TODO! implement that later
        if (loc_of_percent != NULL)
        {
            replace_env_vars(val_buf, val_buf);
        }
        
        // previously if GDF_AppSettings_Get()->verbose_output
        if (true)
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
            value = GDF_Malloc(sizeof(char) * len - 1, GDF_MEMTAG_TEMP_RESOURCE);
            
            strncpy(value, val_buf + 1, len - 2);
            ((char*)value)[len - 2] = '\0';
        }
        if (value == NULL && 
        ((string_reads_true = strcmp(val_buf, "true") == 0) || strcmp(val_buf, "false") == 0))
        {
            if (GDF_AppSettings_Get()->verbose_output)
                LOG_DEBUG("dtype: bool");
            // then is bool value
            dtype = GDF_MAP_DTYPE_BOOL;
            value = GDF_Malloc(sizeof(bool), GDF_MEMTAG_TEMP_RESOURCE);
            *((bool*)value) = string_reads_true ? true : false;
        }
        if (value == NULL && strncmp(val_buf, "{", 1) == 0)
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
                value = GDF_Malloc(sizeof(f64), GDF_MEMTAG_TEMP_RESOURCE);
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
            value = GDF_Malloc(sizeof(i32), GDF_MEMTAG_TEMP_RESOURCE);
            *((i32*)value) = i;
        }
        GDF_AddMapEntry(out_map, GDF_MKEY_FromString(key_buf), value, dtype);
        GDF_Free(value);
        line = strtok(NULL, "\n");
    }
    return true;
}
// max write capacity of 1mb
bool GDF_WriteMapToFile(GDF_Map* map, const char* rel_path)
{
    char* buf = GDF_Malloc(sizeof(char) * MB_BYTES, GDF_MEMTAG_TEMP_RESOURCE);
    if (!GDF_SerializeMap(map, buf))
    {
        LOG_ERR("Failed to serialize map.");
        return false;
    }
    if (!GDF_WriteFile(rel_path, buf))
    {
        LOG_ERR("Failed to write map to file.");
        return false;
    }
    GDF_Free(buf);
    return true;
}
// max read capacity of 1mb
bool GDF_ReadMapFromFile(const char* rel_path, GDF_Map* out_map)
{
    char* buf = GDF_Malloc(sizeof(char) * MB_BYTES, GDF_MEMTAG_TEMP_RESOURCE);
    if (!GDF_ReadFile(rel_path, buf, MB_BYTES))
        return false;
    if (!GDF_DeserializeToMap(buf, out_map))
        return false;
    GDF_Free(buf);
    return true;
}