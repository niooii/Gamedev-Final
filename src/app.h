#pragma once
#include "core.h"
#include "engine/core/logging.h"
#include "engine/core/os/window.h"
#include "engine/core/os/misc.h"
#include "engine/core/os/io.h"
#include "engine/core/subsystems.h"
#include "engine/core/serde/serde.h"
#include "client/client_settings.h"
#include "engine/render/renderer.h"

// TODO! this file and app.c are quite useless put these into main.c

bool GDF_InitApp();
// responsible for initializing directories and files with default values.
// TODO!
bool GDF_InitFirstLaunch();
// returns the time the app ran for in seconds
f64 GDF_RunApp();