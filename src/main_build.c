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

#define NUM_CFILES 500
#define BUILD_PATH "builder_test/"
#define BUILD_CACHE_PATH BUILD_PATH "build_cache/"
#define CHECKSUM_PATH BUILD_CACHE_PATH "checksums/"

bool compute_md5(const char* filename, char* output);
bool read_file_to_string(const char* filename, char* output, size_t max_length);
void write_string_to_file(const char* filename, const char* content);

int main() {
    GDF_InitSubsystems();

    char* c_files[NUM_CFILES];
    char* o_files[NUM_CFILES];
    u32 num_files = 0;

    GDF_Stopwatch* stopwatch = GDF_CreateStopwatch();

    for (int j = 0; j < 1000; j++)
    {
        GDF_DirInfo* dir_info = GDF_GetDirInfo("src");
        if (!dir_info) {
            fprintf(stderr, "Failed to get directory info.\n");
            return 1;
        }

        for (size_t i = 0; i < dir_info->num_nodes; i++) {
            LOG_INFO("%s", dir_info->nodes[i]->full_path);
        }
        GDF_FreeDirInfo(dir_info);
    }

    f64 sec_elapsed = GDF_StopwatchTimeElapsed(stopwatch);
    LOG_INFO("Build finished in %lf seconds.", sec_elapsed);

    if (_mkdir(BUILD_PATH) == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to create build directory.\n");
        return 1;
    }

    if (_mkdir(BUILD_CACHE_PATH) == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to create build cache directory.\n");
        return 1;
    }

    if (_mkdir(CHECKSUM_PATH) == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to create checksum directory.\n");
        return 1;
    }

    for (size_t i = 0; i < num_files; i++) {
        char checksum_file[MAX_PATH_LEN];
        snprintf(checksum_file, MAX_PATH_LEN, CHECKSUM_PATH "%s.md5", strrchr(c_files[i], '\\') + 1);

        char current_checksum[33];
        if (!compute_md5(c_files[i], current_checksum)) {
            fprintf(stderr, "Failed to compute checksum for %s.\n", c_files[i]);
            return 1;
        }

        char stored_checksum[33];
        bool needs_recompile = true;
        if (read_file_to_string(checksum_file, stored_checksum, sizeof(stored_checksum))) {
            if (strcmp(current_checksum, stored_checksum) == 0) {
                needs_recompile = false;
            }
        }

        if (needs_recompile) {
            char command[MAX_PATH_LEN * 2];
            snprintf(command, sizeof(command), "clang -c %s -o %s -g -Wall -Isrc", c_files[i], o_files[i]);
            if (system(command) != 0) {
                fprintf(stderr, "Failed to compile %s.\n", c_files[i]);
                return 1;
            }
            write_string_to_file(checksum_file, current_checksum);
        }
    }

    char link_command[MAX_PATH_LEN * 2] = "clang";
    for (size_t i = 0; i < num_files; i++) {
        strcat(link_command, " ");
        strcat(link_command, o_files[i]);
    }
    strcat(link_command, " -o ./build/assembly.exe -luser32 -lvulkan-1");

    if (system(link_command) != 0) {
        fprintf(stderr, "Failed to link object files.\n");
        return 1;
    }

    printf("Build complete.\n");

    for (size_t i = 0; i < num_files; i++) {
        free(c_files[i]);
        free(o_files[i]);
    }

    return 0;
}

bool compute_md5(const char* filename, char* output) {
    char command[MAX_PATH_LEN];
    snprintf(command, sizeof(command), "certutil -hashfile %s MD5", filename);
    FILE* fp = _popen(command, "r");
    if (fp == NULL) return false;

    char buffer[128];
    bool success = false;
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strstr(buffer, "CertUtil: -hashfile") == NULL && strstr(buffer, "MD5 hash of file") == NULL && strstr(buffer, "\r\n") != buffer) {
            strncpy(output, buffer, 32);
            output[32] = '\0';
            success = true;
            break;
        }
    }
    _pclose(fp);
    return success;
}

bool read_file_to_string(const char* filename, char* output, size_t max_length) {
    FILE* file = fopen(filename, "r");
    if (!file) return false;

    if (fgets(output, max_length, file) == NULL) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

void write_string_to_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "%s", content);
    fclose(file);
}

#endif