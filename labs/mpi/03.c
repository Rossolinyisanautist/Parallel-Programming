#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int W_size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &W_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int token = rank == 0 ? 1 : 0;
	

	if(rank != 0)
	{
		int prev_sosed = rank - 1;
		MPI_Recv(&token, 1, MPI_INT, prev_sosed, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		printf("Precess %d/%d got token %d from %d.\n", rank, W_size, token, prev_sosed);
	}
	int next_sosed = (rank + 1) % W_size;

	printf("Process %d/%d sending token %d to %d.\n", rank, W_size, token, next_sosed);

	MPI_Send(&token, 1, MPI_INT, next_sosed, 0, MPI_COMM_WORLD);

	if(rank == 0)
	{
		int last = W_size - 1;
		MPI_Recv(&token, 1, MPI_INT, last, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		printf("Root got token %d from %d.\n", token, last);
	}


	return 0;

}
