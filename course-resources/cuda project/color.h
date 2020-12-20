#pragma once

#include "platform.h"

typedef struct color
{
    float r, g, b;
} color_t;

#ifdef __NVCC__
__host__ __device__
#endif
static inline color_t* color_init(color_t *color, float r, float g, float b)
{
    color->r = r;
    color->g = g;
    color->b = b;

    return color;
}
