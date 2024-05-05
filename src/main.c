#include "game.h"
#include "core/os/info.h"

int main()
{
    GDF_InitInfo();
    DisplayInfo display_info = GetDisplayInfo();
    GDF_GameConfig config = {
        .spawn_x = GDF_WIN_CENTERED,
        .spawn_y = GDF_WIN_CENTERED,
        .spawn_w = display_info.screen_width * 0.45,
        .spawn_h = display_info.screen_height * 0.5,
        .window_name = "A GDF"
    };
    GDF_StartGame(&config);
    LOG_DEBUG("test logging %f", 3.14f);
    GDF_Run();

    return 0;
}