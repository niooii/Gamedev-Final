#define COMPILE_BUILDER
#ifdef COMPILE_BUILDER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <direct.h>
#include "io.h"
#include "core/subsystems.h"
#include "core/md5.h"
#include "core/serde/serde.h"

#define NUM_CFILES 500
#define BUILD_PATH "builder_test/"
#define BUILD_CACHE_PATH BUILD_PATH "build_cache/"
#define CHECKSUM_FILE BUILD_CACHE_PATH "_checksums.txt"
#define BUILD_OPTIONS_FILE "build_options.gdf"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// All these flags will be in clang format.
// change when we gotta deal with linux too
typedef struct BuildOptions {
    const char* src_dir;
    const char* defines;
    const char* compile_flags;
    const char* linker_flags;
    const char* include_flags;
    const char* profile;
} BuildOptions;
bool load_build_options(BuildOptions* out_opts);
bool save_default_build_options();

// FUCK PLATFORM INDEPENDENCY ILL
// DEAL WITH THAT LATER
bool get_cfile_names(const char *sDir, char* cfile_names);


int main() {
    GDF_InitSubsystems();

    // paths of all c files separated by token '|'
    char* c_files = GDF_Malloc(MAX_PATH_LEN * NUM_CFILES, GDF_MEMTAG_STRING);
    // + 33 (32 bytes for md5, 1 byte for ':')
    char* c_file_checksums = GDF_Malloc((MAX_PATH_LEN + 33) * NUM_CFILES, GDF_MEMTAG_STRING);
    u32 num_files = 0;

    GDF_Stopwatch* stopwatch = GDF_CreateStopwatch();
    bool needs_recompile = false;
    GDF_MakeDir(BUILD_PATH);
    GDF_MakeDir(BUILD_CACHE_PATH);
    GDF_MakeFile(CHECKSUM_FILE);
    BuildOptions* build_options = GDF_Malloc(sizeof(*build_options), GDF_MEMTAG_TEMP_RESOURCE);
    if (GDF_MakeFile(BUILD_OPTIONS_FILE) || !load_build_options(build_options))
    {
        save_default_build_options();
    }
    char path[400];
    GDF_GetAbsolutePath("src", path);
    get_cfile_names(path, c_files);

    // compare files with the stored hashes
    // or just rebuild if the hash doesnt exist
    for (char *c_file = strtok(c_files, "|"); c_file != NULL; c_file = strtok(NULL, "|"))
    {
        char rel_path[400];
        GDF_GetRelativePath(c_file, rel_path);
        u8* md5 = GDF_Malloc(16, GDF_MEMTAG_STRING);
        char* file_buf = GDF_ReadFileExactLen(rel_path);
        md5String(file_buf, md5);
        GDF_Free(file_buf);
        char hash_string[33];   
        for(unsigned int i = 0; i < 16; ++i)
        {
            sprintf(&hash_string[i*2], "%02x", md5[i]);
        }
        LOG_DEBUG("md5 hash: %s", hash_string);

    }

    if (!needs_recompile)
    {
        LOG_INFO("No changes detected. Stopping...");
        f64 sec_elapsed = GDF_StopwatchTimeElapsed(stopwatch);
        LOG_INFO("Build finished in %lf seconds.", sec_elapsed);
        return 0;
    }
    
    f64 sec_elapsed = GDF_StopwatchTimeElapsed(stopwatch);
    LOG_INFO("Build finished in %lf seconds.", sec_elapsed);

    // if (_mkdir(CHECKSUM_PATH) == -1 && errno != EEXIST) {
    //     fprintf(stderr, "Failed to create checksum directory.\n");
    //     return 1;
    // }

    // for (size_t i = 0; i < num_files; i++) {
    //     char checksum_file[MAX_PATH_LEN];
    //     snprintf(checksum_file, MAX_PATH_LEN, CHECKSUM_PATH "%s.md5", strrchr(c_files[i], '\\') + 1);

    //     char current_checksum[33];
    //     if (!compute_md5(c_files[i], current_checksum)) {
    //         fprintf(stderr, "Failed to compute checksum for %s.\n", c_files[i]);
    //         return 1;
    //     }

    //     char stored_checksum[33];
    //     bool needs_recompile = true;
    //     if (read_file_to_string(checksum_file, stored_checksum, sizeof(stored_checksum))) {
    //         if (strcmp(current_checksum, stored_checksum) == 0) {
    //             needs_recompile = false;
    //         }
    //     }

    //     if (needs_recompile) {
    //         char command[MAX_PATH_LEN * 2];
    //         snprintf(command, sizeof(command), "clang -c %s -o %s -g -Wall -Isrc", c_files[i], o_files[i]);
    //         if (system(command) != 0) {
    //             fprintf(stderr, "Failed to compile %s.\n", c_files[i]);
    //             return 1;
    //         }
    //         write_string_to_file(checksum_file, current_checksum);
    //     }
    // }

    // char link_command[MAX_PATH_LEN * 2] = "clang";
    // for (size_t i = 0; i < num_files; i++) {
    //     // strcat(link_command, " ");
    //     // strcat(link_command, o_files[i]);
    // }
    // strcat(link_command, " -o ./build/assembly.exe -luser32 -lvulkan-1");

    // if (system(link_command) != 0) {
    //     fprintf(stderr, "Failed to link object files.\n");
    //     return 1;
    // }

    printf("Build complete.\n");

    // for (size_t i = 0; i < num_files; i++) {
    //     free(c_files[i]);
    //     free(o_files[i]);
    // }

    return 0;
}

bool load_build_options(BuildOptions* out_opts)
{
    GDF_Map* map = GDF_CreateMap();
    return GDF_ReadMapFromFile(BUILD_OPTIONS_FILE, map);
}

bool save_default_build_options()
{
    const char* linker_flags = "-luser32 -lvulkan-1 -L%s/Lib";

    BuildOptions out_opts = {
        .compile_flags = "-g -Wvarargs -Wall",
        .include_flags = "-Isrc",
        .linker_flags = "-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib",
        .defines = "-D_DEBUG -D_CRT_SECURE_NO_WARNINGS",
        .src_dir = "src"
    };

    GDF_Map* map = GDF_CreateMap();
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_COMPILEFLAGS,
        out_opts.compile_flags,
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_INCLUDEFLAGS,
        out_opts.include_flags,
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_LINKERFLAGS,
        out_opts.linker_flags,
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_DEFINES,
        out_opts.defines,
        GDF_MAP_DTYPE_STRING
    );
    GDF_AddMapEntry(
        map, 
        GDF_MKEY_BUILD_SRC_DIR,
        out_opts.src_dir,
        GDF_MAP_DTYPE_STRING
    );

    GDF_WriteMapToFile(map, BUILD_OPTIONS_FILE);
}

bool get_cfile_names(const char *sDir, char* cfile_names)
{
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;

    char sPath[1024];

    sprintf(sPath, "%s\\*.*", sDir);

    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Path not found: [%s]\n", sDir);
        return false;
    }
    do
    {
        //Find first file will always return "."
        //    and ".." as the first two directories.
        if(strcmp(fdFile.cFileName, ".") == 0
                || strcmp(fdFile.cFileName, "..") == 0)
            continue;
        
        sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);

        if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            get_cfile_names(sPath, cfile_names);
        }
        else
        {
            size_t len = strlen(sPath);
            // if c file
            if (sPath[len-2] == '.' && sPath[len-1] == 'c')
            {
                if (cfile_names[0] != '\0')
                {
                    strcat(cfile_names, "|");
                }
                LOG_DEBUG("Found c file: %s", sPath);
                strcat(cfile_names, sPath);
            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); //Find the next file.

    FindClose(hFind); 

    return true;
}
#endif