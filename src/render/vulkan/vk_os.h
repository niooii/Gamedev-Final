#pragma once

#include "core.h"

// Accepts a GDF_LIST of extention names and pushes the required ones.
// Function definition found in implememntations of window.h
void GDF_VK_GetRequiredExtensionNames(const char*** names_list);