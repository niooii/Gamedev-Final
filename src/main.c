#include <core/app.h>

int main()
{
    GDF_AppConfig config = {
        .spawn_x = 200,
        .spawn_y = 200,
        .spawn_w = 200,
        .spawn_h = 200,
        .window_name = "test"
    };
    app_create(&config);
    LOG_DEBUG("test logging %f", 3.14f);
    app_run();
    return 0;
}