#define COMPILE_BUILDER
#ifdef COMPILE_BUILDER

#include "core/subsystems.h"

// more than 500 c files then change for later
// womp womp
#define NUM_CFILES 500

#define BUILD_PATH "builder_test/"
#define BUILD_CACHE_PATH BUILD_PATH "build_cache/"
#define CHECKSUM_PATH BUILD_CACHE_PATH "checksums/"

int main()
{
    char* c_files[NUM_CFILES];
    char* o_files[NUM_CFILES];
    GDF_InitSubsystems();
    if (!GDF_MakeFile("build_settings.gdf"))
    {
        
    }
    GDF_Stopwatch* stopwatch = GDF_CreateStopwatch();
    for (register int i = 0; i < 5000; i++)
    {
        char* outbuf = malloc(10000);
        GDF_ReadFile("resources/icon.ico", outbuf, 10000);
    }
    f64 time_elapsed = GDF_StopwatchTimeElapsed(stopwatch);
    LOG_INFO("Build finished.");
    LOG_DEBUG("Time to build: %lf seconds", time_elapsed);
    return 0;
}

#endif