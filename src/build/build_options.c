#ifdef COMPILE_BUILDER
#include "build_options.h"
#include "core/serde/serde.h"
#include "core/os/io.h"

bool load_build_options(const char* rel_path, BuildOptions* out_opts)
{
    GDF_Map* map = GDF_CreateMap();
    bool read_success = GDF_ReadMapFromFile(rel_path, map);
    if (!read_success)
    {
        return false;
    }
    char* val;
    if ((val = GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_SRC_DIR)) == NULL)
        return false;
    out_opts->src_dir = GDF_StrDup(val);
    if ((val = GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_COMPILEFLAGS)) == NULL)
        return false;
    out_opts->compile_flags = GDF_StrDup(val);
    if ((val = GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_INCLUDEFLAGS)) == NULL)
        return false;
    out_opts->include_flags = GDF_StrDup(val);
    if ((val = GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_LINKERFLAGS)) == NULL)
        return false;
    out_opts->linker_flags = GDF_StrDup(val);
    if ((val = GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_DEFINES)) == NULL)
        return false;
    out_opts->defines = GDF_StrDup(val);
    if ((val = GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_EXECUTABLE_NAME)) == NULL)
        return false;
    out_opts->executable_name = GDF_StrDup(val);
    if ((val = GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_POSTBUILD_COMMAND)) == NULL)
        return false;
    out_opts->post_build_command = GDF_StrDup(val);
    GDF_FreeMap(map);
    return true;
}

bool save_build_options(const char* rel_path, BuildOptions* options)
{
    GDF_Map* map = GDF_CreateMap();
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_COMPILEFLAGS,
        options->compile_flags,
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_INCLUDEFLAGS,
        options->include_flags,
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_LINKERFLAGS,
        options->linker_flags,
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_DEFINES,
        options->defines,
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_SRC_DIR,
        options->src_dir,
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_EXECUTABLE_NAME,
        options->executable_name,
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_POSTBUILD_COMMAND,
        options->post_build_command,
        GDF_MAP_DTYPE_STRING
    );
    bool success = GDF_WriteMapToFile(map, rel_path);
    GDF_FreeMap(map);
    return success;
}

bool save_default_build_options(const char* rel_path)
{
    const char* linker_flags = "-luser32 -lvulkan-1 -L%s/Lib";

    BuildOptions out_opts = {
        .compile_flags = "-g -Wvarargs -Wall -O0",
        .include_flags = "-Isrc -I%VULKAN_SDK%/Include",
        .linker_flags = "-debug /defaultlib:user32.lib /defaultlib:libcmt.lib /defaultlib:vulkan-1.lib -LIBPATH:%VULKAN_SDK%/Lib",
        .defines = "-D_DEBUG -D_CRT_SECURE_NO_WARNINGS",
        .src_dir = "src",
        .executable_name = "EXENAME",
        .post_build_command = ".\\post_build.bat"
    };

    return save_build_options(rel_path, &out_opts);
}
#endif