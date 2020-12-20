#pragma once

#include "color.h"

#include <math.h>

typedef struct sphere
{
    float radius;
    float x, y, z;
    color_t color;
} sphere_t;

static inline sphere_t* sphere_init(sphere_t *sphere, float radius, float x, float y, float z, color_t color)
{
    sphere->radius = radius;
    sphere->x = x;
    sphere->y = y;
    sphere->z = z;
    sphere->color = color;

    return sphere;
}

#ifdef __NVCC__
__device__
#endif
static inline float sphere_test_hit_with_ray_from_origin(sphere_t *sphere, float x, float y, float *out_z)
{
    float dx = x - sphere->x;
    float dy = y - sphere->y;
    float dxSquared = dx * dx;
    float dySquared = dy * dy;
    float radiusSquared = sphere->radius * sphere->radius;

    float ray_t = -INFINITY;
    if (dxSquared + dySquared < radiusSquared) {
        float dz = sqrtf(radiusSquared - dxSquared - dySquared);
        *out_z = dz / sqrtf(radiusSquared);
        ray_t = sphere->z + dz;
    }

    return ray_t;
}

