#include "game/game.h"
#include "core/os/info.h"
#include "app.h"
#include "core/asserts.h"

int main()
{
    GDF_InitInfo();
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
    LOG_DEBUG("test logging %f", 3.14f);
    int x = 3;
    GDF_ASSERT(x == 3);
    GDF_Run();

    return 0;
}