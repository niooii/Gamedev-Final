#include "game/game.h"
#include "core/os/info.h"
#include "app.h"
#include "core/asserts.h"
#include "core/subsystems.h"

int main()
{
    GDF_InitSubsystems();
    const char* testdata = "kugiu4444.2";
    i32 f = 0;
    int filled = sscanf(testdata, "%d", &f);
    if (filled)
    {
        LOG_INFO("HEHEHEHAW %d", f);
    }
    DisplayInfo display_info = GetDisplayInfo();
    GDF_AppConfig config = {
        .spawn_x = GDF_WIN_CENTERED,
        .spawn_y = GDF_WIN_CENTERED,
        .spawn_w = display_info.screen_width * 0.45,
        .spawn_h = display_info.screen_height * 0.5,
        .window_name = "A GDF",
        .show_console = true
    };
    GDF_InitApp(&config);
    GDF_Run();

    return 0;
}