#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
void* thread_start(void* arg) 
{
	printf("hello from thread %u\n", *((unsigned int*) arg));
	return NULL;
}

int main(int argc, char** argv) 
{
	unsigned int thread_count = 0;
	scanf("%u", &thread_count);

	pthread_t* threads = malloc(sizeof(pthread_t) * thread_count);
	unsigned int* args = malloc(sizeof(unsigned int) * thread_count);

	for(unsigned int i = 0; i < thread_count; ++i) {
		*(args + i) = i;
		pthread_create(threads + i, NULL, thread_start, (void*) (args + i));
	}

	for(unsigned int i = 0; i < thread_count; ++i) {
		pthread_join(*(threads + i), NULL);
	}

	free(threads);
	threads = NULL;

	free(args);
	args = NULL;
	return 0;
}
