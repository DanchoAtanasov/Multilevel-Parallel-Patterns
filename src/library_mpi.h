// library_mpi.h
#ifndef _LIBRARY_MPI_H_
#define _LIBRARY_MPI_H_

//#include <iostream>
#include "mpi.h"
#include <stdio.h>

template<typename R, typename... Args, typename... AArgs>
int farm(R(*worker)(Args...), float* m1, float* m2, float* m3) {
    printf("In farm\n");
	int numtasks, rank, sendcount, recvcount, source;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	MPI_Request reqs[3];
	MPI_Status stats[3];

	int payload;

	if (rank == 0) {
		payload = 42;
	}

	source = 0;
	sendcount = 1;
	recvcount = 1;

	// Broadcasting the whole matrix2 to all nodes
	MPI_Ibcast(&payload, 1, MPI_INT, source, MPI_COMM_WORLD, &reqs[0]);

	// Wait for messages to be received
	MPI_Waitall(1, reqs, stats);

	printf("sup %d\n", payload);
	int res = (*worker);

	MPI_Finalize();

	return res;
}

#endif
