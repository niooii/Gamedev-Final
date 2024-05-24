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
#define BUILD_PATH "builder_test\\"
#define BUILD_CACHE_PATH BUILD_PATH "build_cache\\"
#define CHECKSUM_FILE BUILD_CACHE_PATH "_checksums.txt"
#define BUILD_OPTIONS_FILE "build_options.gdf"
#define BUILT_WITH_OPTIONS_FILE BUILD_CACHE_PATH "used_options.gdf"

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
bool load_build_options(const char* rel_path, BuildOptions* out_opts);
bool save_build_options(const char* rel_path, BuildOptions* options);
bool save_default_build_options();

// FUCK PLATFORM INDEPENDENCY ILL
// DEAL WITH THAT LATER
bool get_cfile_names(const char *sDir, char* cfile_names);
// returns pointer to the first character of hash, which spans 32 characters.
// DOES NOT ALLOCATE MEMORY
// returns NULL on failure.
char* get_checksum(const char* checksums_str, const char* cfile_rel_path);

int main() {
    GDF_InitSubsystems();

    // paths of all c files separated by token '|'
    char* c_files = GDF_Malloc(MAX_PATH_LEN * NUM_CFILES, GDF_MEMTAG_STRING);
    // paths of new directories that have been made to avoid repeated directory
    // creation attempts. delimiter: '|'
    char* created_dirs = GDF_Malloc(MAX_PATH_LEN * NUM_CFILES, GDF_MEMTAG_STRING);
    char* build_cache_abs_path = GDF_Malloc(MAX_PATH_LEN, GDF_MEMTAG_STRING);
    GDF_GetAbsolutePath(BUILD_CACHE_PATH, build_cache_abs_path);
    // + 33 (32 bytes for md5, 1 byte for ':')
    const c_file_checksums_len = (MAX_PATH_LEN + 33) * NUM_CFILES;
    char* c_file_checksums = GDF_Malloc(c_file_checksums_len, GDF_MEMTAG_STRING);
    GDF_ReadFile(CHECKSUM_FILE, c_file_checksums, c_file_checksums_len);
    u32 files_compiled = 0;    

    GDF_Stopwatch* stopwatch = GDF_CreateStopwatch();
    bool needs_recompile = false;
    GDF_MakeDir(BUILD_PATH);
    GDF_MakeDir(BUILD_CACHE_PATH);
    GDF_MakeFile(CHECKSUM_FILE);
    BuildOptions* build_options = GDF_Malloc(sizeof(*build_options), GDF_MEMTAG_TEMP_RESOURCE);
    BuildOptions* prev_built_with = GDF_Malloc(sizeof(*build_options), GDF_MEMTAG_TEMP_RESOURCE);
    if (GDF_MakeFile(BUILD_OPTIONS_FILE) || !load_build_options(BUILD_OPTIONS_FILE, build_options))
    {
        save_default_build_options();
        load_build_options(BUILD_OPTIONS_FILE, build_options);
    }
    if (GDF_MakeFile(BUILT_WITH_OPTIONS_FILE) || !load_build_options(BUILT_WITH_OPTIONS_FILE, build_options))
    {
        save_build_options(BUILT_WITH_OPTIONS_FILE, build_options);
        prev_built_with = build_options;
    }
    if (prev_built_with != build_options)
    {
        // we need to check for differences in the build options
        // TODO!
    }
    char path[400];
    GDF_GetAbsolutePath("src", path);
    get_cfile_names(path, c_files);

    // compare files with the stored hashes
    // or just rebuild if the hash doesnt exist
    for (char *c_file = strtok(c_files, "|"); c_file != NULL; c_file = strtok(NULL, "|"))
    {
        // get c file's relative path 
        char rel_path[400];
        GDF_GetRelativePath(c_file, rel_path);

        // produce file md5 checksum
        u8* md5 = GDF_Malloc(16, GDF_MEMTAG_STRING);
        LOG_DEBUG("%s", rel_path);
        char* file_buf = GDF_ReadFileExactLen(rel_path);
        md5String(file_buf, md5);
        GDF_Free(file_buf);
        char new_checksum[33];   
        for(unsigned int i = 0; i < 16; ++i)
        {
            sprintf(&new_checksum[i*2], "%02x", md5[i]);
        }

        bool needs_compile = false;

        // compare stored checksum in file vs the just calculated one
        char* stored_cs_p;
        if ((stored_cs_p = get_checksum(c_file_checksums, rel_path)) == NULL)
        {
            LOG_DEBUG("checksum not found, compiling and storing checksum.");
            needs_compile = true;
        }
        else
        {
            char stored_checksum[33];
            LOG_DEBUG("md5 hash: %s", new_checksum);
            snprintf(stored_checksum, 33, stored_cs_p);
            LOG_DEBUG("stored hash: %s", stored_checksum);
            if (strcmp(stored_checksum, new_checksum) == 0)
            {
                // same checksum, dont do shit 
                LOG_DEBUG("Checksums match, skipping recompilation");
                continue;
            }
            needs_compile = true;
        }

        // handle compilation if needed
        if (needs_compile)
        {
            char* compile_command = GDF_Malloc(2000, GDF_MEMTAG_STRING);
            // get absolute path of soon-to-be object file
            char o_file[1024];
            strcpy(o_file, build_cache_abs_path);
            strcat(o_file, rel_path);
            o_file[strlen(o_file) - 1] = 'o';

            // create directories if they dont exist
            // example o_file = 
            // "E:\Projects\C\gamedev_final\builder_test\build_cache\src\game\settings\keybinds.o"
            char target_dir[1024];
            strcpy(target_dir, o_file);
            char* last_backslash = strrchr(target_dir, '\\');
            *(last_backslash + 1) = '\0';
            // example target_dir:
            // "E:\Projects\C\gamedev_final\builder_test\build_cache\src\core\serde\"
            char* dirs_rel = target_dir + strlen(build_cache_abs_path);
            // example dirs_rel:
            // "src\game\world\"
            // TODO!

            // assemble and run the compile command
            sprintf(
                compile_command, 
                "clang -c %s %s %s %s -o %s",
                c_file,
                build_options->compile_flags,
                build_options->defines,
                build_options->include_flags,
                o_file
            );
            LOG_INFO("Compiling \"%s\"", rel_path);
            system(compile_command);
            files_compiled++;
            GDF_Free(compile_command);
        }
        
    }

    if (files_compiled == 0)
    {
        LOG_INFO("No changes detected. Stopping...");
        f64 sec_elapsed = GDF_StopwatchTimeElapsed(stopwatch);
        LOG_INFO("Build finished in %lf seconds.", sec_elapsed);
        return 0;
    }
    
    f64 sec_elapsed = GDF_StopwatchTimeElapsed(stopwatch);
    LOG_INFO("Build finished in %lf seconds.", sec_elapsed);
    GDF_Free(c_files);
    GDF_Free(c_file_checksums);

    printf("Build complete.\n");

    return 0;
}

bool load_build_options(const char* rel_path, BuildOptions* out_opts)
{
    GDF_Map* map = GDF_CreateMap();
    bool read_success = GDF_ReadMapFromFile(rel_path, map);
    if (!read_success)
    {
        return false;
    }
    out_opts->src_dir = GDF_StrDup(GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_SRC_DIR));
    out_opts->compile_flags = GDF_StrDup(GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_COMPILEFLAGS));
    out_opts->include_flags = GDF_StrDup(GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_INCLUDEFLAGS));
    out_opts->linker_flags = GDF_StrDup(GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_LINKERFLAGS));
    out_opts->defines = GDF_StrDup(GDF_MAP_GetValueString(map, GDF_MKEY_BUILD_DEFINES));
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
    bool success = GDF_WriteMapToFile(map, rel_path);
    GDF_FreeMap(map);
    return success;
}

bool save_default_build_options()
{
    const char* linker_flags = "-luser32 -lvulkan-1 -L%s/Lib";

    BuildOptions out_opts = {
        .compile_flags = "-g -Wvarargs -Wall -O0",
        .include_flags = "-Isrc",
        .linker_flags = "-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib",
        .defines = "-D_DEBUG -D_CRT_SECURE_NO_WARNINGS",
        .src_dir = "src"
    };

    return save_build_options(BUILD_OPTIONS_FILE, &out_opts);
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

char* get_checksum(const char* checksums_str, const char* cfile_rel_path)
{
    char* checksum = strstr(checksums_str, cfile_rel_path);
    if (checksum == NULL)
        return NULL;
    // advance pointer forward by the length of the path + ":", 
    // only getting the checksum
    checksum += (strlen(cfile_rel_path) + 1);
    return checksum;
}

#endif