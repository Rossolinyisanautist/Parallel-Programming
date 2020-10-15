#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define NUM_COUNT (4 * 1024 * 1024)
typedef int num_t;

static const int Max_rand = 5;
static const int Benchmarks = 50;
static num_t Array[NUM_COUNT];

static void fill_arr_with_randoms(num_t arr[], size_t len, int max_rand) 
{
	srand(1); 
	for(size_t i = 0; i < len; ++i) {
		arr[i] = (num_t)  ((double)rand()  / ((double)RAND_MAX + 1) * max_rand);
	}
	
}

static num_t sum_1(int nums[], size_t len) 
{
	int res = 0;

	for(int i = 0; i < Benchmarks; i++) {
		for(size_t j = 0; j < len - 1; j++) {
			res += nums[j];
			res += nums[j];
			res += nums[j + 1];
			res += nums[j + 1];
		}
	}	
	return res;
}


static num_t sum_2(int nums[], size_t len) 
{
	num_t a = 0, b = 0, c = 0, d = 0;

	for(int i = 0; i < Benchmarks; i++) {
		for(size_t j = 0; j < len - 1; j++) {
			a += nums[j];
			b += nums[j];
			c += nums[j + 1];
			d += nums[j + 1];
		}
	}	
	return a + b + c + d;
}


int main(int argc, char** argv) 
{
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <type of the approach to sum random numbers(1 or 2)>\n", argv[0]);
		exit(1);
	}

	fill_arr_with_randoms(Array, NUM_COUNT, Max_rand);

	num_t sum = 0;

	int type = (int) strtol(argv[1], NULL, 10);

	switch(type) {
		case 1:
			sum = sum_1(Array, NUM_COUNT);
			break;
		case 2:
			sum = sum_2(Array, NUM_COUNT);
			break;
		default:
			fputs("Invalid approach selected. Select 1 or 2.\n", stderr);
			exit(1);
	}
	printf("The sum of an array of random number is %lld\n", (long long) sum);

	return EXIT_SUCCESS;
}
