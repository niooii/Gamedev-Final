#include <core/app.h>

int main()
{
    app_config config = {
        .spawn_x = 200,
        .spawn_y = 200,
        .spawn_w = 200,
        .spawn_h = 200,
        .window_name = "test"
    };
    app_create(&config);
    GDF_DEBUG("test %f", 3.14f);
    GDF_FATAL("a GG");
    app_run();
    return 0;
}