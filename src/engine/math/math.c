#include "math.h"

#include "core/os/sysinfo.h"

static bool RAND_SEEDED = FALSE;

i32 random() 
{
    if (!RAND_SEEDED) 
    {
        srand((u32)GDF_GetAbsoluteTime());
        RAND_SEEDED = TRUE;
    }
    return rand();
}

i32 random_in_range(i32 min, i32 max) 
{
    if (!RAND_SEEDED)
    {
        srand((u32)GDF_GetAbsoluteTime());
        RAND_SEEDED = TRUE;
    }
    return (rand() % (max - min + 1)) + min;
}

f32 frandom() 
{
    return (float)random() / (f32)RAND_MAX;
}

f32 frandom_in_range(f32 min, f32 max) 
{
    return min + ((float)random() / ((f32)RAND_MAX / (max - min)));
}