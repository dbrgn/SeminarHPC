/*
 * heat.c -- MPI implementation of a solver for the heat equation
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <getopt.h>
#include <common.h>

/**
 * \brief main function
 */
int	main(int argc, char *argv[]) {
	int	rank;
	int	ierr;
	int	num_procs;
	MPI_Status	status;
	int	tag = 1;

	// initialize MPI
	ierr = MPI_Init(&argc, &argv);

	// get MPI dimension parameters
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	char	rankprefix[10];
	snprintf(rankprefix, sizeof(rankprefix), "%d", rank);

	// parse the command line
	int	c;
	int	n = 10;
	while (EOF != (c = getopt(argc, argv, "n:")))
		switch (c) {
		case 'n':
			n = atoi(optarg);
			break;
		}

	// process zero creates the matrix
	float	*A = NULL;
	float	*Ai = NULL;
	if (rank == 0) {
	}

	double	start = gettime();

	// process 0 has to send the data to all the other processes
	if (rank == 0) {
		//MPI_Send(buffer, h * n, MPI_FLOAT, r, tag, MPI_COMM_WORLD);
	} else {
		// receive my part of the matrix
		//ierr = MPI_Recv(buffer, height * n, MPI_FLOAT, 0, tag,
		//	MPI_COMM_WORLD, &status);
		//int	count;
		//MPI_Get_count(&status, MPI_FLOAT, &count);
	}
	tag++;

	// start the solver algorithm

		// send the pivot row to all other processes, as a side effect,
		// all processes are synchronized on this point
		//MPI_Bcast(p, 2 * n, MPI_FLOAT, sender, MPI_COMM_WORLD);

	// the computation is now complete, so rank zero has to collect all
	// the pieces
	if (rank == 0) {
		// receive remaining data from other processes
		//	MPI_Recv(buffer, h * n, MPI_FLOAT, r, tag,
		//		MPI_COMM_WORLD, &status);
	} else {
		// send the data to process 0
		//ierr = MPI_Send(buffer, height * n, MPI_FLOAT, 0, tag,
		//	MPI_COMM_WORLD);
	}

	// measure end time
	double	end = gettime();

	// we are now done, process 0 displays the result
	if (rank == 0) {
		printf("%d,%.6f,%d\n", n, end - start, num_procs);
	}

	// cleanup MPI
	MPI_Finalize();

	return EXIT_SUCCESS;
}
