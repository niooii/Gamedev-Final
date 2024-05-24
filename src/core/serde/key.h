#pragma once

#include "core.h"
#include <string.h>
#include <stdio.h>

// define map keys here
#define FOREACH_KEY(F)\
/* custom build tool */\
    F(GDF_MKEY_BUILD_SRC_DIR)\
    F(GDF_MKEY_BUILD_DEFINES)\
    F(GDF_MKEY_BUILD_COMPILEFLAGS)\
    F(GDF_MKEY_BUILD_LINKERFLAGS)\
    F(GDF_MKEY_BUILD_INCLUDEFLAGS)\
    F(GDF_MKEY_BUILD_PROFILE)\
    F(GDF_MKEY_BUILD_EXECUTABLE_NAME)\
/* actual stuff */\
    F(GDF_MKEY_APP_SETTINGS_APP_TYPE)\
    F(GDF_MKEY_APP_SETTINGS_VERBOSE_OUTPUT)\
    F(GDF_MKEY_APP_SETTINGS_CLIENT_SHOW_CONSOLE)\
    F(GDF_MKEY_SETTINGS_DEV_CAN_FLY)\
    F(GDF_MKEY_SETTINGS_DEV_NOCLIP)\
    F(GDF_MKEY_SETTINGS_DEV_DRAW_WIREFRAME)\
    F(GDF_MKEY_NUM_KEYS)\
    F(GDF_MKEY_ERROR_KEY)\

#define GEN_ENUM(ENUM) ENUM,
// +9 skips the first 9 characters omitting GDF_MKEY_
#define GEN_STRING(STRING) #STRING + 9,

typedef enum GDF_MKEY {
    FOREACH_KEY(GEN_ENUM)
} GDF_MKEY;

// returns NULL on failure
void GDF_MKEY_ToString(GDF_MKEY key, char* out_str);
// returns GDF_MKEY_ERROR_KEY on failure
GDF_MKEY GDF_MKEY_FromString(const char* str);