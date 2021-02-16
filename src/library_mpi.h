// library_mpi.h
#ifndef _LIBRARY_MPI_H_
#define _LIBRARY_MPI_H_

//#include <iostream>
#include "mpi.h"
#include <stdio.h>

int farm() {
	int numtasks, rank, sendcount, recvcount, source;

	MPI_Init(&argc, &argv);
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
	MPI_Ibcast(payload, 1, MPI_INT, source, MPI_COMM_WORLD, &reqs[0]);

	// Wait for messages to be received
	MPI_Waitall(1, reqs, stats);

	printf("sup %d", payload);

	MPI_Finalize();

	return 0;
}

#endif
