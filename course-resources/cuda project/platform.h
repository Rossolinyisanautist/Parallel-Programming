#pragma once

#ifdef __NVCC__

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#define dev_size_t int
#define dev_image_channel_t int

#define PRIds "d"
#define PRIic "d"

#else

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#define dev_size_t size_t
#define dev_image_channel_t uint8_t

#define PRIds "zu"
#define PRIic PRIu8

#endif /* __NVCC__ */
