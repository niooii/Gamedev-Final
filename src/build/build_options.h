#ifdef COMPILE_BUILDER
#include "def.h"

// All these flags will be in lld-link format.
typedef struct BuildOptions {
    const char* src_dir;
    const char* defines;
    const char* compile_flags;
    const char* linker_flags;
    const char* include_flags;
    const char* executable_name;
    const char* profile;
} BuildOptions;
bool load_build_options(const char* rel_path, BuildOptions* out_opts);
bool save_build_options(const char* rel_path, BuildOptions* options);
bool save_default_build_options(const char* rel_path);

#endif