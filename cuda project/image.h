#pragma once

#include "color.h"
#include "platform.h"
#include "util.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct image
{
    dev_size_t width, height;
    dev_size_t channels_per_pixel;
    dev_size_t data_size;
    dev_image_channel_t *data;
} image_t;

static image_t* image_init(image_t *image, dev_size_t width, dev_size_t height)
{
    image->width = width;
    image->height = height;
    image->channels_per_pixel = 3;
    image->data_size = (dev_size_t) (sizeof(*image->data) * width * height * image->channels_per_pixel);

    image->data = (dev_image_channel_t *) malloc((size_t) image->data_size);
    if (image->data != NULL) {
        memset(image->data, 0, image->data_size);
    }

    return image;
}

#ifdef __NVCC__
__host__ __device__
#endif
static inline void image_set_pixel(image_t *image, dev_size_t x, dev_size_t y, color_t color)
{
    dev_size_t linear_position =
        y * image->width * image->channels_per_pixel + x * image->channels_per_pixel;

    image->data[linear_position++] =
        (dev_image_channel_t) (util_clamp(color.r, 0.0f, 1.0f) * 255.0f);
    image->data[linear_position++] =
        (dev_image_channel_t) (util_clamp(color.g, 0.0f, 1.0f) * 255.0f);
    image->data[linear_position++] =
        (dev_image_channel_t) (util_clamp(color.b, 0.0f, 1.0f) * 255.0f);
    
}

static inline void image_print_as_ppm(image_t *image)
{
    puts("P3");
    printf("%" PRIds " %" PRIds "\n", image->width, image->height);
    puts("255");

    dev_size_t end = image->width * image->height * image->channels_per_pixel;
    for (dev_size_t i = 0; i < end; i += image->channels_per_pixel) {
        printf(
            "%" PRIic " %" PRIic " %" PRIic "\n",
            image->data[i],
            image->data[i + 1],
            image->data[i + 2]
        );
    }
}

static inline void image_free_data(image_t *image)
{
    free(image->data);
    image->data = NULL;
}
