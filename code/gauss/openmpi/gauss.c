/*
 * gauss.c -- MPI implementation of the Gauss algorithm
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
#include <common.h>

int	main(int argc, char *argv[]) {
	int	rank;
	int	ierr;
	int	num_procs;
	MPI_Status	status;
	int	tag = 1;

	// initialize MPI
	ierr = MPI_Init(&argc, &argv);

	// get MPI 
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	char	rankprefix[10];
	snprintf(rankprefix, sizeof(rankprefix), "%d", rank);

	// parse the command line
	int	c;
	int	n = 10;
	while (EOF != (c = getopt(argc, argv, "p:n:")))
		switch (c) {
		case 'n':
			n = atoi(optarg);
			break;
		case 'p':
			matrix_precision = atoi(optarg);
			break;
		}

	// process zero creates the matrix
	float	*A = NULL;
	float	*Ai = NULL;
	if (rank == 0) {
		A = random_float_matrix(n, n);
		Ai = float_unit_matrix(n);

		// display the initialized matrix
		if (n <= 10) {
			matrix_precision = 6;
			display_float_matrix(stdout, A, n, n);
			matrix_precision = 3;
		}
	}
	matrix_prefix = rankprefix;

	// initialize data structures that each process wants to use
	float	blocksize = n / (float)num_procs;
	int	minrow = round(rank * blocksize);
	int	maxrow = round((rank + 1) * blocksize);
	int	height = maxrow - minrow;
	float	*a = (float *)malloc(2 * n * height * sizeof(float));
	for (int i = minrow; i < maxrow; i++) {
		for (int j = 0; j < n; j++) {
			a[2 * n * (i - minrow) + n + j] = (i == j) ? 1 : 0;
		}
	}
	float	*p = (float *)malloc(2 * n * sizeof(float)); // pivot line

	// create a buffer that we can use for block transfers
	float	*buffer = (float *)malloc(round(blocksize + 1) * n * sizeof(float));
	double	start = gettime();

	if (rank == 0) {
		// copy data from a to the matrix
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < n; j++) {
				a[j + 2 * n * i] = A[j + n * i];
			}
		}

		// send rest of the data to all the other processors
		for (int r = 1; r < num_procs; r++) {
			int	fromrow = round(r * blocksize);
			int	torow = round((r + 1) * blocksize);
			int	h = torow - fromrow;
			memcpy(buffer, A + fromrow * n, h * n * sizeof(float));
			MPI_Send(buffer, h * n, MPI_FLOAT, r, tag, MPI_COMM_WORLD);
		}
	} else {
		// receive my part of the matrix
		ierr = MPI_Recv(buffer, height * n, MPI_FLOAT, 0, tag,
			MPI_COMM_WORLD, &status);
		int	count;
		MPI_Get_count(&status, MPI_FLOAT, &count);
		//printf("%d: got %d of %d floats\n", rank, count, height * n);
		for (int i = minrow; i < maxrow; i++) {
			for (int j = 0; j < n; j++) {
				a[j + 2 * n * (i - minrow)]
					= buffer[j + n * (i -  minrow)];
			}
		}
	}
	tag++;

	// start the gauss algorithm
	int	i = 0;
	while (i < n) {
		// find out which process is going to compute the pivot row
		int	sender;
		for (sender = 0; sender < num_procs; sender++) {
			if ((round(sender * blocksize) <= i) && (i < round((sender + 1) * blocksize))) {
				break;
			}
		}

		// find out whether we are responsible for computing the pivot line
		if (sender == rank) {
			// compute the pivot row
			float	pivot = a[2 * n * (i - minrow) + i];
			for (int j = 0; j < 2 * n; j++) {
				a[2 * n * (i - minrow) + j] /= pivot;
				p[j] = a[2 * n * (i - minrow) + j];
			}
		}

		// send the pivot row to all other processes
		MPI_Bcast(p, 2 * n, MPI_FLOAT, sender, MPI_COMM_WORLD);

		// now perform the computation
		for (int k = minrow; k < maxrow; k++) {
			if (k != i) {
				float	b = a[i + 2 * n * (k - minrow)];
				for (int j = i; j < 2 * n; j++) {
					a[j + 2 * n * (k - minrow)] -= b * p[j];
				}
			}
		}

		// increment row counter
		i++;
	}

	// the computation is now complete, so rank zero has to collect all
	// the pieces
	if (rank == 0) {
		// copy data into the result array
		for (int i = 0; i < maxrow; i++) {
			for (int j = 0; j < n; j++) {
				Ai[j + i * n] = a[j + n + 2 * n * i];
			}
		}

		// receive remaining data from other processes
		for (int r = 1; r < num_procs; r++) {
			int	fromrow = round(r * blocksize);
			int	torow = round((r + 1) * blocksize);
			int	h = torow - fromrow;
			MPI_Recv(buffer, h * n, MPI_FLOAT, r, tag, MPI_COMM_WORLD, &status);
			for (int i = fromrow; i < torow; i++) {
				for (int j = 0; j < n; j++) {
					Ai[j + n * i] = buffer[j + n * (i - fromrow)];
				}
			}
		}
	} else {
		for (int i = minrow; i < maxrow; i++) {
			for (int j = 0; j < n; j++) {
				buffer[j + n * (i - minrow)] = a[j + n + 2 * n * (i - minrow)];
			}
		}
		ierr = MPI_Send(buffer, height * n, MPI_FLOAT, 0, tag,
			MPI_COMM_WORLD);
	}

	double	end = gettime();

	// we are now down, process 0 displays the result
	if (rank == 0) {
		printf("%d,%.6f,%d\n", n, end - start, num_procs);
		if (n <= 10) {
			matrix_prefix = NULL;
			matrix_precision = 6;
			display_float_matrix(stdout, Ai, n, n);
			matrix_prefix = rankprefix;
		}
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
