#pragma once

#include "color.h"
#include "image.h"
#include "platform.h"
#include "sphere.h"
#include "util.h"

#include <math.h>
#include <stdlib.h>

static sphere_t* spheres_create_random(
                     dev_size_t count,
                     float min_radius, float max_radius,
                     float min_x, float max_x,
                     float min_y, float max_y,
                     float min_z, float max_z
                 )
{
    sphere_t *spheres = (sphere_t *) malloc(sizeof(*spheres) * count);

    for (dev_size_t i = 0; i < count; ++i) {
        color_t random_color;
        color_init(
            &random_color,
            util_random(0.0f, 1.0f),
            util_random(0.0f, 1.0f),
            util_random(0.0f, 1.0f)
        );

        sphere_init(
            spheres + i,
            util_random(min_radius, max_radius),
            util_random(min_x, max_x),
            util_random(min_y, max_y),
            util_random(min_z, max_z),
            random_color
        );
    }

    return spheres;
}

#ifndef __NVCC__

static void spheres_raytrace(sphere_t *spheres, dev_size_t sphere_count, image_t out_image)
{
    for (dev_size_t y = 0; y < out_image.height; ++y) {
        for (dev_size_t x = 0; x < out_image.height; ++x) {
            float ray_origin_x = (float) x;
            float ray_origin_y = (float) y;

            color_t color;
            color_init(&color, 0, 0, 0);

            float max_z = -INFINITY;
            for (dev_size_t i = 0; i < sphere_count; ++i) {
                sphere_t *sphere = &spheres[i];

                float out_z;
                float ray_t =
                    sphere_test_hit_with_ray_from_origin(
                        sphere,
                        ray_origin_x, ray_origin_y,
                        &out_z
                    );

                if (ray_t > max_z) {
                    max_z = ray_t;

                    color_t *sphere_color = &sphere->color;
                    color_init(
                        &color,
                        sphere_color->r * out_z,
                        sphere_color->g * out_z,
                        sphere_color->b * out_z
                    );
                }
            }

            image_set_pixel(&out_image, x, y, color);
        }
    }
}

#else

__global__ static void spheres_raytrace(sphere_t* spheres, dev_size_t sphere_count, image_t out_image)
{
    /* TODO: Extract the body of the nested loop of the serial raytracing
             function and put it here. Figure out how to convert threadIdx,
             blockIdx, blockDim into an x and y coordinates of your image. Use
             x and y to do calculations as usual. If the x and y is outside of
             the image space, do nothing and return from the kernel right away. */
	dev_size_t x = blockIdx.x * blockDim.x + threadIdx.x;
    dev_size_t y = blockIdx.y * blockDim.y + threadIdx.y;

	float ray_origin_x = (float) x;
	float ray_origin_y = (float) y;

	color_t color;
	color_init(&color, 0, 0, 0); // device func

	float max_z = -INFINITY;
	for (dev_size_t i = 0; i < sphere_count; ++i) {
		sphere_t *sphere = &spheres[i];

		float out_z;
		float ray_t =
			sphere_test_hit_with_ray_from_origin(
				sphere,
				ray_origin_x, ray_origin_y,
				&out_z
			);

		if (ray_t > max_z) {
			max_z = ray_t;

			color_t *sphere_color = &sphere->color;
			color_init(
				&color,
				sphere_color->r * out_z,
				sphere_color->g * out_z,
				sphere_color->b * out_z
			);
		}
	}
	// fine because func is __device__
	image_set_pixel(&out_image, x, y, color);
}

#endif /* __NVCC__ */

static inline void spheres_free(sphere_t *spheres)
{
    free(spheres);
}
