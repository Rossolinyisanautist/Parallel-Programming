#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


static int nums_1[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10
};

static int nums_2[] = {
	10, 20, 30, 40, 50, 60, 70, 80, 90, 100
};

static int sum = 0;

typedef struct thread_args 
{
	int* numbers;
	size_t count;
} thread_args_t;

void* thread_start(void* arg) 
{	
	thread_args_t* args = (thread_args_t*) arg;
	int* numbers = args->numbers;
	size_t count = args->count;

	while(count--) {
		sum += *numbers++;	
	}
	return NULL;
}

int main(int argc, char** argv) 
{
	pthread_t threads[2];
	thread_args_t args[2];
	
	args[0].numbers = nums_1;
	args[0].count  = sizeof(nums_1) / sizeof(nums_1[0]);

	args[1].numbers = nums_2;
	args[1].count  = sizeof(nums_2) / sizeof(nums_2[0]);


	for(int i = 0; i < 2; ++i) {
		pthread_create(threads + i, NULL, thread_start, args + i);	
	}

	for(int i = 0; i < 2; ++i) {
		pthread_join(*(threads + i), NULL);
	}

	printf("sum  = %d\n", sum);

	return 0;
}
