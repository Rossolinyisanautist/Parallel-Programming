
#include "bmp.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <immintrin.h>

#if !defined C_IMPLEMENTATION && \
    !defined SIMD_INTRINSICS_IMPLEMENTATION && \
    !defined SIMD_ASM_IMPLEMENTATION
#define SIMD_INTRINSICS_IMPLEMENTATION 1
#endif

int main(int argc, char *argv[])
{
    int result = EXIT_FAILURE;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <brightness> <contrast> <source file> <dest. file>\n", argv[0]);
        return result;
    }

    float brightness = strtof(argv[1], NULL);
    float contrast = strtof(argv[2], NULL);

    char *source_file_name = argv[3];
    char *destination_file_name = argv[4];
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


	printf("start\n");
        for (size_t position = 0; position < channels_count; position += step) {
#if defined C_IMPLEMENTATION

            pixels[position] =
                (uint8_t) UTILS_CLAMP(pixels[position] * contrast + brightness, 0.0f, 255.0f);
            pixels[position + 1] =
                (uint8_t) UTILS_CLAMP(pixels[position + 1] * contrast + brightness, 0.0f, 255.0f);
            pixels[position + 2] =
                (uint8_t) UTILS_CLAMP(pixels[position + 2] * contrast + brightness, 0.0f, 255.0f);

#elif defined SIMD_INTRINSICS_IMPLEMENTATION

            /*
                Write the intrinsics for the SIMD assembly bellow in
                SIMD_ASM_IMPLEMENTATION here in SIMD_INTRINSICS_IMPLEMENTATION.
            */

            // TODO
		// broadcast 32-bit sp float into zmm
		__m512 zmm_brightness = _mm512_set1_ps(brightness);


		// broadcast contrast to zmm
		__m512 zmm_contrast   = _mm512_set1_ps(contrast);


		// load 16 8-bit unsigned integers from memory
		void const* pixels_addr = pixels + position;
		// for some reason gcc cant find _mm_loadu_epi8(pixels_addr);
		__m128i xmmi_16pixels  = _mm_load_si128( (__m128i*) pixels_addr);


		// zero extend these 16 8-bit unsigned integers into 16 32-bit signed integers
		__m512i zmmi_16pixels = _mm512_cvtepu8_epi32(xmmi_16pixels);


		// convert 16 32-bit signed integers into 16 32-bit floats
		__m512 zmm_16pixels = _mm512_cvtepi32_ps(zmmi_16pixels);


		// mult and add
		__m512 zmm_result = _mm512_fmadd_ps(zmm_16pixels, zmm_contrast, zmm_brightness);


		// convert 16 32-bit floats into 16 32-bit integers
		__m512i zmmi_result = _mm512_cvtps_epi32(zmm_result);


		// create less or equal mask for 16 32-bit integers
		//__m512i zmmi_number_255 = _mm512_set1_epi32(255);
		//__mmask16 mask_le_255 = _mm512_cmple_epi32_mask(zmmi_result, zmmi_number_255);	


		// clump below 255
		//__m512i zmmi_res_clump_up = _mm512_mask_blend_epi32(mask_le_255, zmmi_number_255, zmmi_result);


		// create greater or equal mask
		//__m512i zmmi_number_0 = _mm512_set1_epi32(0);
		//__mmask16 mask_ge_0 = _mm512_cmpge_epi32_mask(zmmi_res_clump_up, zmmi_number_0);


		// clump above 0
		//__m512i zmmi_res_clumped = _mm512_mask_blend_epi32(mask_ge_0, zmmi_number_0, zmmi_res_clump_up);
	
		// load back to memory. Did not knew about saturation that is why implemented clumping
		__m128i xmmi_result = _mm512_cvtusepi32_epi8(zmmi_result);	
		_mm_store_si128( (void*) pixels_addr, xmmi_result);
		
#elif defined SIMD_ASM_IMPLEMENTATION

            __asm__ __volatile__ (
                "vbroadcastss (%0), %%zmm2\n\t"
                "vbroadcastss (%1), %%zmm1\n\t"
                "vpmovzxbd (%2,%3), %%zmm0\n\t"
                "vcvtdq2ps %%zmm0, %%zmm0\n\t"
                "vfmadd132ps %%zmm1, %%zmm2, %%zmm0\n\t"
                "vcvtps2dq %%zmm0, %%zmm0\n\t"
                "vpmovusdb %%zmm0, (%2,%3)\n\t"
            ::
                "S"(&brightness), "D"(&contrast), "b"(pixels), "c"(position)
            :
                "%zmm0", "%zmm1", "%zmm2"
            );

#endif
        }

	printf("done.\n");		
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
