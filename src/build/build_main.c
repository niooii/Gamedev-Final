#ifdef COMPILE_BUILDER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <direct.h>
#include "io.h"
#include "core/subsystems.h"
#include "md5.h"
#include "build_options.h"
#include "checksums.h"
#include "core/serde/serde.h"

#define NUM_CFILES 500
#define CFILES_STR_LEN NUM_CFILES * MAX_PATH_LEN
#define BUILD_PATH "build\\"
#define BUILD_CACHE_PATH BUILD_PATH "build_cache\\"

#define LAST_BUILD_STATUS_PATH BUILD_CACHE_PATH "last_build_status.txt"
#define BUILD_STATUS_SUCCESS "success"
#define BUILD_STATUS_LINK_FAIL "link_fail"
#define BUILD_STATUS_COMPILE_FAIL "compile_fail"
#define BUILD_STATUS_POST_BUILD_FAIL "postbuild_fail"

#define CHECKSUM_FILE BUILD_CACHE_PATH "_checksums.txt"
#define BUILD_OPTIONS_FILE "build_options.gdf"
#define BUILT_WITH_OPTIONS_FILE BUILD_CACHE_PATH "used_options.gdf"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// FUCK PLATFORM INDEPENDENCY ILL
// DEAL WITH THAT LATER
bool get_cfile_names(const char *sDir, char* cfile_names);

static u32 files_compiled = 0;

int main(int argc, char *argv[]) {
    GDF_InitSubsystems(0);
    // paths of all c files separated by token '|'
    char* c_files = GDF_Malloc(CFILES_STR_LEN, GDF_MEMTAG_STRING);
    // paths of all c files separated by token ' '
    // to be used by linker command
    char* o_files = GDF_Malloc(CFILES_STR_LEN, GDF_MEMTAG_STRING);
    // paths of new directories that have been made to avoid repeated directory
    // creation attempts. delimiter: '|'
    char* created_dirs = GDF_Malloc(CFILES_STR_LEN, GDF_MEMTAG_STRING);
    char* build_cache_abs_path = GDF_Malloc(MAX_PATH_LEN, GDF_MEMTAG_STRING);
    GDF_GetAbsolutePath(BUILD_CACHE_PATH, build_cache_abs_path);

    bool success = false;

    GDF_Stopwatch* stopwatch = GDF_Stopwatch_Create();

    // create a bunch of files and load build options
    GDF_MakeDir(BUILD_PATH);
    GDF_MakeDir(BUILD_CACHE_PATH);
    GDF_MakeFile(CHECKSUM_FILE);
    GDF_MakeFile(LAST_BUILD_STATUS_PATH);
    const char* last_build_status = GDF_ReadFileExactLen(LAST_BUILD_STATUS_PATH);
    bool should_relink = strcmp(last_build_status, BUILD_STATUS_LINK_FAIL) == 0;
    bool should_run_post_build = strcmp(last_build_status, BUILD_STATUS_POST_BUILD_FAIL) == 0;
    bool should_recompile_all = false;
    // should be success if nothing happens anyways
    GDF_WriteFile(LAST_BUILD_STATUS_PATH, BUILD_STATUS_SUCCESS);
    // ----
    BuildOptions* build_options = GDF_Malloc(sizeof(*build_options), GDF_MEMTAG_TEMP_RESOURCE);
    BuildOptions* prev_built_with = GDF_Malloc(sizeof(*prev_built_with), GDF_MEMTAG_TEMP_RESOURCE);
    
    const char* build_options_path = BUILD_OPTIONS_FILE;

    for (u16 i = 0; i < argc; i++)
    {
        const char* arg = argv[i];

        if (strcmp(arg, "-rebuild") == 0)
        {
            should_recompile_all = true;
            should_run_post_build = true;
            continue;
        }
        if (strncmp(arg, "-F", 2) == 0)
        {
            const char* override_opt_file = arg + 2;
            build_options_path = override_opt_file;
            // LOG_INFO("new path: %s", build_options_path);
            continue;
        }
    }
    LOG_WARN("STAGE 1");
    if (GDF_MakeFile(build_options_path) || !load_build_options(build_options_path, build_options))
    {
        save_default_build_options(build_options_path);
        load_build_options(build_options_path, build_options);
        LOG_WARN("LOADED DEFAULT BUILD OPTIONS..");
    }
    
    // if recompile_all is true the changes wouldnt matter anyways
    if (!should_recompile_all)
    {
        if (GDF_MakeFile(BUILT_WITH_OPTIONS_FILE) || !load_build_options(BUILT_WITH_OPTIONS_FILE, prev_built_with))
        {
            save_build_options(BUILT_WITH_OPTIONS_FILE, build_options);
            prev_built_with = build_options;
        }
        // if the build options and previously built with options
        // were loaded separately (on first creation they both share)
        // the same memory
        if (prev_built_with != build_options)
        {
            // we need to check for differences in the build options
            if (
                strcmp(build_options->compile_flags, prev_built_with->compile_flags) != 0
                || strcmp(build_options->defines, prev_built_with->defines) != 0
            )
            {
                should_recompile_all = true;
                LOG_INFO("Build options changed, recompiling all files...");
            }
            if (
                strcmp(build_options->linker_flags, prev_built_with->linker_flags) != 0
                || strcmp(build_options->executable_name, prev_built_with->executable_name) != 0
            )
            {
                should_relink = true;
                LOG_INFO("Build options changed, files will be linked again...");
            }
        }

        if (!should_run_post_build)
        {
            if (
                strcmp(build_options->post_build_command, prev_built_with->post_build_command) != 0
            )
            {
                should_run_post_build = true;
                LOG_INFO("Post-build command changed, new one will be ran...");
            }
        }
        GDF_Free(prev_built_with);
    }

    if (should_recompile_all)
    {
        // purge the checksums file
        GDF_WriteFile(CHECKSUM_FILE, "");
    }

    // + 33 (32 bytes for md5, 1 byte for ':')
    const size_t c_file_checksums_len = (MAX_PATH_LEN + 33) * NUM_CFILES;
    char* c_file_checksums = GDF_Malloc(c_file_checksums_len, GDF_MEMTAG_STRING);
    GDF_ReadFile(CHECKSUM_FILE, c_file_checksums, c_file_checksums_len);
    
    char path[400];
    GDF_GetAbsolutePath(build_options->src_dir, path);
    char extern_path[400];
    GDF_GetAbsolutePath("extern", extern_path);
    // ----
    get_cfile_names(path, c_files);
    // grab from extern dir
    // THIS NEEDS A HUGE REWORK.
    get_cfile_names(extern_path, strrchr(c_files, '|') + 1);
    LOG_DEBUG("%s", c_files);

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
        if (should_recompile_all)
        {
            goto compile_stage;
        }
        bool needs_compile = false;

        // compare stored checksum in file vs the just calculated one
        char stored_checksum[33];
        bool has_stored_checksum = get_checksum(c_file_checksums, rel_path, stored_checksum);
        
        if (!has_stored_checksum)
        {
            LOG_DEBUG("checksum not found, compiling and storing checksum.");
            LOG_DEBUG("current md5 hash: %s", new_checksum);
            needs_compile = true;
        }
        else
        {
            LOG_DEBUG("current md5 hash: %s", new_checksum);
            LOG_DEBUG("stored hash: %s", stored_checksum);
            if (strcmp(stored_checksum, new_checksum) == 0)
            {
                // same checksum, dont do shit 
                LOG_DEBUG("Checksums match, skipping recompilation");
            }
            else
            {
                needs_compile = true;
            }
        }

        // o_file is added HERE in case
        // we deleted a c file.
        // if object files were grabbed
        // from the directory itself
        // it could still be linking
        // with object files from 
        // deleted c files.
        compile_stage:
        char o_file[MAX_PATH_LEN];
        strcpy(o_file, build_cache_abs_path);
        strcat(o_file, rel_path);
        o_file[strlen(o_file) - 1] = 'o';
        
        strcat(o_files, o_file);
        strcat(o_files, " ");

        // handle compilation if needed
        if (needs_compile || should_recompile_all)
        {
            char* compile_command = GDF_Malloc(2048, GDF_MEMTAG_STRING);

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
            // holds the character previously replaced
            // by the null terminator
            // so we can replace it back later
            char previous;
            char* backslash_pos;
            while ((backslash_pos = strchr(dirs_rel, '\\')) != NULL)
            {
                // store character after the backslash
                previous = *(backslash_pos + 1);
                // set that character to null terminator 
                *(backslash_pos + 1) = '\0';
                // target_dir will now be the directory
                // we gotta make. check if it is already
                // made or else create it.
                if (strstr(created_dirs, target_dir) == NULL)
                {
                    GDF_MakeDirAbs(target_dir);
                    LOG_DEBUG("Created directory %s", target_dir);
                    strcat(created_dirs, target_dir);
                    strcat(created_dirs, "|");
                }
                // LOG_INFO("yes %s", target_dir);
                // put old character back into the string
                *(backslash_pos + 1) = previous;
                // update dirs_rel such that the next
                // call to strchr will return the
                // next backslash pointer, not the same one
                dirs_rel = backslash_pos + 1;
            }
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
            int result = system(compile_command);
            if (result != 0)
            {
                LOG_ERR("Failed to compile \"%s\". Stopping...", rel_path);
                GDF_WriteFile(CHECKSUM_FILE, c_file_checksums);
                GDF_MakeFile(BUILT_WITH_OPTIONS_FILE);
                save_build_options(BUILT_WITH_OPTIONS_FILE, build_options);
                GDF_WriteFile(LAST_BUILD_STATUS_PATH, BUILD_STATUS_COMPILE_FAIL);
                GDF_Free(compile_command);
                goto build_stage_end;
            }
            // upd checksum adds if there isnt one already
            update_checksum(c_file_checksums, rel_path, new_checksum);
            files_compiled++;
            GDF_Free(compile_command);
        }
        
    }

    if (files_compiled == 0)
    {
        LOG_INFO("No changes detected.");
    }
    else
    {
        LOG_INFO("Updating checksums...");
        GDF_WriteFile(CHECKSUM_FILE, c_file_checksums);
        // LOG_DEBUG("o files: %s", o_files);
    }
    if (files_compiled != 0 || should_relink)
    {
        LOG_INFO("Linking files...");
        char* link_command = GDF_Malloc(CFILES_STR_LEN, GDF_MEMTAG_STRING);
        char executable_path[MAX_PATH_LEN];
        strcpy(executable_path, BUILD_PATH);
        strcat(executable_path, build_options->executable_name);
        strcat(executable_path, ".exe");
        sprintf(
            link_command,
            "lld-link -out:%s %s %s",
            executable_path,
            o_files,
            build_options->linker_flags
        );

        LOG_DEBUG("Link command: %s", link_command);

        if (system(link_command) != 0)
        {
            LOG_ERR("Linking failed, stopping...");
            save_build_options(BUILT_WITH_OPTIONS_FILE, build_options);
            GDF_WriteFile(LAST_BUILD_STATUS_PATH, BUILD_STATUS_LINK_FAIL);
            GDF_Free(link_command);
            goto build_stage_end;
        }
        GDF_Free(link_command);
    }

    success = true;
    
    build_stage_end:
    GDF_Free(c_files);
    GDF_Free(c_file_checksums);
    GDF_Free(o_files);
    GDF_Free(build_cache_abs_path);
    save_build_options(BUILT_WITH_OPTIONS_FILE, build_options);
    
    if (success)
    {
        f64 sec_elapsed = GDF_Stopwatch_TimeElasped(stopwatch);
        if (files_compiled > 0)
        {
            LOG_INFO("Compiled %d files in %lf seconds.", files_compiled, sec_elapsed);
            LOG_INFO("Finished building \"%s.exe\" successfully.", build_options->executable_name);
            // TODO! right now only reruns post-build when a file changes for some reason
            // idk a better method unlucky just run it manually igs
            should_run_post_build = true;
        }
        else 
        {
            GDF_Free(build_options);
            return 0;
        }
    }

    if (should_run_post_build)
    {
        GDF_Stopwatch_Reset(stopwatch);
        // run post build routine
        LOG_INFO("Running post-build routine...");
        if (strlen(build_options->post_build_command) == 0)
        {
            LOG_INFO("No post-build routine to run, finished.");
        }
        else if (system(build_options->post_build_command) != 0)
        {
            LOG_ERR("Post-build routine failed in %lf seconds.", GDF_Stopwatch_TimeElasped(stopwatch));
            GDF_WriteFile(LAST_BUILD_STATUS_PATH, BUILD_STATUS_POST_BUILD_FAIL);
            GDF_Free(build_options);
            return 1;
        }

        LOG_INFO("Post-build routine finished in %lf seconds.", GDF_Stopwatch_TimeElasped(stopwatch));

        return 0;
    }

    f64 sec_elapsed = GDF_Stopwatch_TimeElasped(stopwatch);
    LOG_ERR("Build failed in %lf seconds.", sec_elapsed);
    LOG_ERR("Failed to build \"%s.exe\"", build_options->executable_name);
    GDF_Free(build_options);
    return 1;
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