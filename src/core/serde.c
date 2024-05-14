#include "serde.h"

bool GDF_SerializeMap(GDF_Map* map, char* out_buf)
{
    char key_str[200];
    u32 map_len = 0; // TODO!
    for (int i = 0; i < GDF_MKEY_NUM_KEYS; i++)
    {
        if (map->entries[i] == NULL)
            continue;
        GDF_MKEY_ToString(map->entries[i]->key, key_str);

    }
}

bool GDF_DeserializeMap(char* data, GDF_Map* out_map)
{

}