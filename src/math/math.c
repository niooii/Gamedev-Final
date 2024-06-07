#include "math.h"

#include "core/os/info.h"
#include <math.h>

static bool RAND_SEEDED = FALSE;

f32 gsin(f32 x) 
{
    return sinf(x);
}

f32 gcos(f32 x) 
{
    return cosf(x);
}

f32 gtan(f32 x) 
{
    return tanf(x);
}

f32 gacos(f32 x) 
{
    return acosf(x);
}

f32 gsqrt(f32 x) 
{
    return sqrtf(x);
}

f32 gabs(f32 x) 
{
    return fabsf(x);
}

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