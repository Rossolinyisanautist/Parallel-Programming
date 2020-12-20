#include <time.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int wsize, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &wsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	srand(time(0));
	int work_time = rand() % 5;
	printf("Process %d/%d doing job for %d seconds.\n", rank, wsize, work_time);
	sleep(work_time);

	MPI_Barrier(MPI_COMM_WORLD);

	if(rank == 0)
	{
		puts("All processes are syncd.");
	}
	return 0;
}
