#include "serde.h"

// Function to replace %ENV_VAR% with the value of the environment variable
// MAY CAUSE MEMORY LEAK
// if none found out_buf = NULL and returns.
static void replace_env_vars(const char* input, char* out_buf) {
    // the pointer of where we are in the input string in terms of
    // copying over parts of the string to the out_buf
    char* cpy_ptr = input;
    // the pointer to the current percent sign being evaluated
    char* prev_percent = strchr(input, '%');
    char* current_percent = prev_percent;
    if (prev_percent == NULL)
    {
        out_buf = NULL;
        return;
    }
    i32 percents_found = 1;
    while ((current_percent = strchr(current_percent + 1, '%')) != NULL)
    {
        if (++percents_found % 2 != 0)
        {
            prev_percent = current_percent;
            continue;
        }

        // a pair of percent signs was found
        u32 var_name_len = current_percent - prev_percent - 1;
        // TODO! do i use a static array here or nah wtf
        char* var_name = GDF_Malloc(var_name_len + 1, GDF_MEMTAG_STRING);
        strncpy(var_name, prev_percent + 1, var_name_len);
        // LOG_INFO("ENV VAR NAME: %s", var_name);

        // points to static data so should not call free.
        const char* var_value = getenv(var_name);
        GDF_Free(var_name);
        if (var_value == NULL)
            var_value = "";
        // LOG_INFO("ENV VAR VALUE: %s", var_value);
        
        // copy the stuff from the cpy pointer to the first percent
        u32 prev_percent_offset = prev_percent - cpy_ptr;
        strncat(out_buf, cpy_ptr, prev_percent_offset);
        // skip the stuff inside first and second % signs
        cpy_ptr = current_percent + 1;
        // copy the value to where the %env_name% stuff was supposed to be
        strcat(out_buf, var_value);
        // on next iteration, everything from the current percent to the next prev percent
        // will be copied.

        prev_percent = current_percent;
    }
    if (percents_found <= 1)
    {
        out_buf = NULL;
        return;
    }
    // if no more iterations can happen, copy the rest over.
    strcat(out_buf, cpy_ptr);
}

// TODO!: the serialization of a map entry that
// has loaded an environment variable 
// should output the environment variable
// in proper form: key=%env_name% when serialized
bool GDF_SerializeMap(GDF_Map* map, char* out_buf)
{
    char line_buf[650];
    char key_buf[150];
    char val_buf[500];
    for (u32 i = 0; i < GDF_MKEY_NUM_KEYS; i++)
    {
        if (map->entries[i] == NULL)
            continue;
        memset(line_buf, 0, 650);
        memset(key_buf, 0, 150);
        memset(val_buf, 0, 500);
        
        GDF_MKEY_ToString(i, key_buf);
        // LOG_DEBUG("first check: key buf contains %s", key_buf);
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
        // LOG_DEBUG("second check: key buf contains %s", key_buf);
        snprintf(line_buf, 200, "%s=%s\n", key_buf, val_buf);
        // LOG_WARN("line buf contains: %s", line_buf);
        strcat(out_buf, line_buf);
    }

    LOG_DEBUG("Serialized map: \n%s", out_buf);
    return true;
}

bool GDF_DeserializeToMap(char* data, GDF_Map* out_map)
{
    // iterate through lines
    const char* line = strtok(data, "\n");
    u32 line_num = 0;
    char* line_buf = GDF_Malloc(650, GDF_MEMTAG_STRING);    
    char* key_buf = GDF_Malloc(150, GDF_MEMTAG_STRING); 
    char* val_buf = GDF_Malloc(500, GDF_MEMTAG_STRING); 
    while(line) {
        LOG_DEBUG("line is %s", line);
        line_num++;
        GDF_MemZero(line_buf, 650);
        GDF_MemZero(key_buf, 150);
        GDF_MemZero(val_buf, 500);
        sscanf(line, "%[^=]=%[^\n]", key_buf, val_buf);

        char* loc_of_percent = strchr(val_buf, '%');
        // escape character will be backslash or something
        // but TODO! implement that later
        if (loc_of_percent != NULL)
        {
            char tmp_val_buf[500];
            GDF_MemZero(tmp_val_buf, 500);
            replace_env_vars(val_buf, tmp_val_buf);
            strcpy(val_buf, tmp_val_buf);
        }
        
        // previously if GDF_AppSettings_Get()->verbose_output
        if (GDF_AppSettings_Get()->verbose_output)
        {
            // LOG_DEBUG("key: %s", key_buf);
            // LOG_DEBUG("value: %s", val_buf);
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
            LOG_DEBUG("value buf contents: %s", val_buf);
            
            value = GDF_Malloc(len, GDF_MEMTAG_TEMP_RESOURCE);
            LOG_DEBUG("allocated %d bytes for value.", len);
            LOG_DEBUG("allocated %d bytes for value.", strlen(val_buf));
            LOG_DEBUG("value addr: %p", value);
            
            strncpy(value, val_buf + 1, len - 2);
            LOG_DEBUG("copied str");
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
                LOG_ERR("found bad value (%s) at line %d, stopping map deserialization...", line_buf, line_num);
                GDF_Free(line_buf);
                GDF_Free(key_buf);
                GDF_Free(val_buf);
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
    GDF_Free(line_buf);
    GDF_Free(key_buf);
    GDF_Free(val_buf);
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
    char* buf = GDF_ReadFileExactLen(rel_path);
    if (!GDF_DeserializeToMap(buf, out_map))
    {
        return false;
    }
    GDF_Free(buf);
    return true;
}