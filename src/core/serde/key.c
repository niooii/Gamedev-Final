#include "key.h"

static const char* mkey_strings[] = {
    FOREACH_KEY(GEN_STRING)
};

void GDF_MKEY_ToString(GDF_MKEY key, char* out_str)
{
    strcpy(out_str, mkey_strings[key]);
    return;
}

GDF_MKEY GDF_MKEY_FromString(const char* str)
{
    for (int i = 0; i < GDF_MKEY_NUM_KEYS; i++) {
        if (strcmp(mkey_strings[i], str) == 0)
            return i;
    }
    LOG_ERR("Requested map key does not exist: %s", str);
    return GDF_MKEY_ERROR_KEY;
}