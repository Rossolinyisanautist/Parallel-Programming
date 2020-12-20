#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>


static double* generate_randoms(size_t count)
{
	double* res = malloc(sizeof(*res) * count);
	for(size_t i = 0; i < count; ++i)
	{
		res[i] = (double) rand();
	}

	return res;
}

static double calc_avg(double* nums, size_t count)
{
	double res = 0;
	while(*nums)
	{
		res += *nums++;
	}
	res /= count;
	return res;
}


int main(int argc, char** argv)
{
	
	return 0;
}
