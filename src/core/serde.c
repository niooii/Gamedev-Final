#include "serde.h"

bool GDF_SerializeMap(GDF_Map* map, char* out_buf)
{
    u32 map_len = 0; // TODO!
    for (int i = 0; i < GDF_MKEY_NUM_KEYS; i++)
    {
        if (map->entries[i] == NULL)
            continue;
        char key_str[200];
        
        GDF_MKEY_ToString(i, key_str);

    }
}

bool GDF_DeserializeMap(char* data, GDF_Map* out_map)
{

}