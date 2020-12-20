#pragma once

#include <math.h>
#include <stdlib.h>

static inline float util_random(float min, float max)
{
    return min + (float) rand() / (float) RAND_MAX * (max - min);
}

#ifdef __NVCC__
__host__ __device__
#endif
static inline float util_clamp(float x, float min, float max)
{
    return fminf(fmaxf(x, min), max);
}
