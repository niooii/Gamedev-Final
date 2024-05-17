#pragma once
#include "core.h"
#include "core/logging.h"
#include "core/os/window.h"
#include "core/os/info.h"
#include "core/os/io.h"
#include "core/subsystems.h"
#include "core/serde/serde.h"
#include "app_settings.h"
// TODO! this file and app.c are quite useless put these into main.c

bool GDF_InitApp();
// responsible for initializing directories and files with default values.
// TODO!
bool GDF_InitFirstLaunch();
bool GDF_RunApp();