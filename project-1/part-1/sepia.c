#include "bmp.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <immintrin.h>

#if !defined C_IMPLEMENTATION && \
    !defined SIMD_INTRINSICS_IMPLEMENTATION && \
    !defined SIMD_ASM_IMPLEMENTATION
#define C_IMPLEMENTATION 1
#endif

int main(int argc, char *argv[])
{
    int result = EXIT_FAILURE;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <source file> <dest. file>\n", argv[0]);
        return result;
    }

    char *source_file_name = argv[1];
    char *destination_file_name = argv[2];
    FILE *source_descriptor = NULL;
    FILE *destination_descriptor = NULL;

    bmp_image image; bmp_init_image_structure(&image);

    source_descriptor = fopen(source_file_name, "r");
    if (source_descriptor == NULL) {
        fprintf(stderr, "Failed to open the source image file '%s'\n", source_file_name);
        goto cleanup;
    }

    const char *error_message;
    bmp_open_image_headers(source_descriptor, &image, &error_message);
    if (error_message != NULL) {
        fprintf(stderr, "Failed to process the image '%s':\n\t%s\n", source_file_name, error_message);
        goto cleanup;
    }

    bmp_read_image_data(source_descriptor, &image, &error_message);
    if (error_message != NULL) {
        fprintf(stderr, "Failed to process the image '%s':\n\t%s\n", source_file_name, error_message);
        goto cleanup;
    }

    destination_descriptor = fopen(destination_file_name, "w");
    if (destination_descriptor == NULL) {
        fprintf(stderr, "Failed to create the output image '%s'\n", destination_file_name);
        goto cleanup;
    }

    bmp_write_image_headers(destination_descriptor, &image, &error_message);
    if (error_message != NULL) {
        fprintf(stderr, "Failed to process the image '%s':\n\t%s\n", destination_file_name, error_message);
        goto cleanup;
    }

    /* Main Image Processing Loop */
    {
        uint8_t *pixels = image.pixels;

        size_t width = image.absolute_image_width;
        size_t height = image.absolute_image_height;

        size_t channels_count = width * height * 4;

#if defined SIMD_INTRINSICS_IMPLEMENTATION || defined SIMD_ASM_IMPLEMENTATION
        size_t step = 16;
#else
        size_t step = 4;
#endif

        for (size_t position = 0; position < channels_count; position += step) {
#if defined C_IMPLEMENTATION

            static const float Sepia_Coefficients[] = {
                0.272f, 0.534f, 0.131f,
                0.349f, 0.686f, 0.168f,
                0.393f, 0.769f, 0.189f
            };

            uint32_t blue =
                pixels[position];
            uint32_t green =
                pixels[position + 1];
            uint32_t red =
                pixels[position + 2];

            pixels[position] =
                (uint8_t) UTILS_MIN(
                              Sepia_Coefficients[0] * blue  +
                              Sepia_Coefficients[1] * green +
                              Sepia_Coefficients[2] * red,
                              255.0f
                          );
            pixels[position + 1] =
                (uint8_t) UTILS_MIN(
                              Sepia_Coefficients[3] * blue  +
                              Sepia_Coefficients[4] * green +
                              Sepia_Coefficients[5] * red,
                              255.0f
                          );
            pixels[position + 2] =
                (uint8_t) UTILS_MIN(
                              Sepia_Coefficients[6] * blue  +
                              Sepia_Coefficients[7] * green +
                              Sepia_Coefficients[8] * red,
                              255.0f
                          );

#elif defined SIMD_INTRINSICS_IMPLEMENTATION

            static const float Sepia_Coefficients[] = {
                0.272f, 0.349f, 0.393f, 1.0f, 0.272f, 0.349f, 0.393f, 1.0f, 0.272f, 0.349f, 0.393f, 1.0f, 0.272f, 0.349f, 0.393f, 1.0f,
                0.534f, 0.686f, 0.769f, 1.0f, 0.534f, 0.686f, 0.769f, 1.0f, 0.534f, 0.686f, 0.769f, 1.0f, 0.534f, 0.686f, 0.769f, 1.0f,
                0.131f, 0.168f, 0.189f, 1.0f, 0.131f, 0.168f, 0.189f, 1.0f, 0.131f, 0.168f, 0.189f, 1.0f, 0.131f, 0.168f, 0.189f, 1.0f
            };

            __m512 coeff1 = _mm512_load_ps(&Sepia_Coefficients[0]);
            __m512 coeff2 = _mm512_load_ps(&Sepia_Coefficients[16]);
            __m512 coeff3 = _mm512_load_ps(&Sepia_Coefficients[32]);
            __m512i ints = _mm512_cvtepu8_epi32(_mm_load_si128((__m128i *) &pixels[position]));
            __m512 floats = _mm512_cvtepi32_ps(ints);
            __m512 temp1 = floats;
            __m512 temp2 = floats;
            __m512 temp3 = floats;
            temp1 = _mm512_permute_ps(temp1, 0b11000000); // 11 - last 32 bits(alpha), 00 - take first 32 bits(blue) 3 times
            temp2 = _mm512_permute_ps(temp2, 0b11010101); // 01 - take second 32 bits(green) 3 times
            temp3 = _mm512_permute_ps(temp3, 0b11101010); // 10 - take third 32 bits(red) 3 times
            floats = _mm512_mul_ps(coeff1, temp1);
            floats = _mm512_fmadd_ps(coeff2, temp2, floats);
            floats = _mm512_fmadd_ps(coeff3, temp3, floats);
            ints = _mm512_cvtps_epi32(floats);
            _mm512_mask_cvtusepi32_storeu_epi8(&pixels[position], 0xffff, ints);

#elif defined SIMD_ASM_IMPLEMENTATION

            /*
                Write the inline assembly representation of the intrinsics above
                in SIMD_INTRINSICS_IMPLEMENTATION here in SIMD_ASM_IMPLEMENTATION.
            */
		
            static const float coeffs[] = {
                0.272f, 0.349f, 0.393f, 1.0f, 0.272f, 0.349f, 0.393f, 1.0f, 0.272f, 0.349f, 0.393f, 1.0f, 0.272f, 0.349f, 0.393f, 1.0f,
                0.534f, 0.686f, 0.769f, 1.0f, 0.534f, 0.686f, 0.769f, 1.0f, 0.534f, 0.686f, 0.769f, 1.0f, 0.534f, 0.686f, 0.769f, 1.0f,
                0.131f, 0.168f, 0.189f, 1.0f, 0.131f, 0.168f, 0.189f, 1.0f, 0.131f, 0.168f, 0.189f, 1.0f, 0.131f, 0.168f, 0.189f, 1.0f
            };
            // TODO

		// load first 16 coeffs for blue channels of 4 pixels
		// load second line of 16 coeff for green channels of 4 pixels
		// load third line of 16 coeffs for red channel of 4 pixels
		// load 16 8-bit integers from memory to xmmi and then zero extend to zmmi		
		// convert 16 32-bit integers in zmmi into 16 32-bit floats in zmm
		// load blue channels of 4 pixels in 	temp1	: [  b1,   b1,  b1,  a1		 b2,   b2,  b2,  a2	... ] *
		 				     //	coef1	: [ .272, .349 .393, 1		.272, .349 .393, 1	... ] +

		// load green of all 4 pixels in 	temp2	: [  g1,   g1,  g1,  a1		 g2,   g2,  g2,  a2	... ] *
		 				     //	coef2	: [ .543, .686 .769, 1		.543, .686 .769, 1	... ] +

		// load red of all 4 pixels in 		temp2	: [  r1,   r1,  r1,  a1		 r2,   r2,  r2,  a2	... ] *
		 				     //	coef3	: [ .131, .168 .189, 1		.131, .168 .189, 1	... ] = sepia filtered BGR

		// multiply blue channels of all 4 pixels by their coefficients		:	(temp1*coef1)

		// mult green channels of all 4 pixels by their coefficients 
		//		and add result of multiplication of blue by its coefs	:	(temp2*coef2 + temp1*coef1)

		// mult red channels of all 4 pixels by their coefficients
		//		and add result of multiplication of blue by its coefs
		//		and add result of multiplication of green by its coefs	:	(temp3*coef3 + temp2*coef2 + temp1*coef1)
		// convert 16 32-bit float into 16 32-bit integers
		// narrow with saturation 16 32-bit integers to 16 8-bit integers
		// consecutively write 16 8-bit integers back to memory
		 __asm__ __volatile__ (
		"vmovups (%0), %%zmm1\n\t"			// coef1 = zmm1
		"vmovups 64(%0), %%zmm2\n\t"			// coef2 = zmm2
		"vmovups 128(%0), %%zmm3\n\t"			// coef3 = zmm3
		"vpmovzxbd (%3,%4), %%zmm0\n\t"			// ints  = zmm0
		"vcvtdq2ps %%zmm0, %%zmm0\n\t"			// floats = zmm0
		"vpermilps $0b11000000, %%zmm0, %%zmm4\n\t"	// temp1 = zmm4
		"vpermilps $0b11010101, %%zmm0, %%zmm5\n\t"	// temp2 = zmm5
		"vpermilps $0b11101010, %%zmm0, %%zmm6\n\t"	// temp3 = zmm6
		"vmulps %%zmm4, %%zmm1, %%zmm0\n\t"
		"vfmadd132ps %%zmm5, %%zmm0, %%zmm2\n\t"	
		"vfmadd132ps %%zmm6, %%zmm2, %%zmm3\n\t"
		"vcvtps2dq %%zmm3, %%zmm3\n\t"
                "vpmovusdb %%zmm3, (%3,%4)\n\t"
            ::
                "S"(coeffs), "D"(coeffs+16), "b"(coeffs+32), "c"(pixels), "d"(position)
            :
                 /*floats*/ "%zmm0", /*coefs*/ "%zmm1", "%zmm2","%zmm3", /*temps*/ "%zmm4", "%zmm5", "%zmm6"
            );

#endif
        }
    }

    bmp_write_image_data(destination_descriptor, &image, &error_message);
    if (error_message != NULL) {
        fprintf(stderr, "Failed to process the image '%s':\n\t%s\n", destination_file_name, error_message);
        goto cleanup;
    }

    result = EXIT_SUCCESS;

cleanup:
    bmp_free_image_structure(&image);

    if (source_descriptor != NULL) {
        fclose(source_descriptor);
        source_descriptor = NULL;
    }

    if (destination_descriptor != NULL) {
        fclose(destination_descriptor);
        destination_descriptor = NULL;
    }

    return result;
}
