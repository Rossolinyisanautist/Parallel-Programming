#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) 
{
	MPI_Init(&argc, &argv);

	int world_size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0)
	{
		puts("Hello from root\nWaiting for the messages from children.");
		
#define MAX_MSG_SIZE 1024
		static char msg[MAX_MSG_SIZE];
		for(int i = 1; i < world_size; i++)
		{
			MPI_Recv(msg, MAX_MSG_SIZE, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Message from child %d: \t%s\n", i, msg);
		}
	}
	else 
	{
		printf("Hello from %d/%d child\n", rank, world_size);
		puts("Sending message to root");

		static char msg[] = "hello";
		MPI_Send(msg, sizeof(msg), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;

}
