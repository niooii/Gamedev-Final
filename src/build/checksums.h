#ifdef COMPILE_BUILDER
#include "def.h"

bool get_checksum(const char* checksums_str, const char* cfile_rel_path, char* out_checksum);
bool update_checksum(char* checksums_str, const char* cfile_rel_path, const char* new_checksum);
bool add_checksum(char* checksums_str, const char* cfile_rel_path, const char* checksum);

#endif