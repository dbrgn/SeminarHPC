/*
 * gauss.c -- Gauss Algorithmus, konventionelle Implementation
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <pthread.h>
#include <common.h>

#ifdef DOUBLE
#define	F	double
#define display_matrix	display_double_matrix
#define random_matrix	random_double_matrix
#else
#define	F	float
#define display_matrix	display_float_matrix
#define random_matrix	random_float_matrix
#endif

// global data
F	*a;
int	n;

/**
 * \brief threaded Gauss algorithm implementation
 */
void	gauss() {
	double	start = gettime();
	int	i = 0;
	do {
		// divide pivot row by the pivot elemnt
		F	pivot = M(a, 2 * n, i, i); // pivot element
		for (int j = i; j < 2 * n; j++) {
			M(a, 2 * n, i, j) /= pivot;
		}

		// parallel loop: perform row operations all over the matrix
#pragma omp parallel for
		for (int k = 0; k < n; k++) {
			if (k != i) {
				F	b = M(a, 2 * n, k, i);
				for (int j = i; j < 2 * n; j++) {
					M(a, 2 * n, k, j) -= b * M(a, 2 * n, i, j);
				}
			}
		}

		// go to the next pivot element
		i++;
	} while (i < n);
	double	end = gettime();
	printf("%d,%.6f\n", n, end - start);
	fflush(stdout);
}

/**
 * \brief perform a gauss experiment with n x n matrix
 *
 * \param n	dimension of the matrix the inverse
 */
void	experiment(int n) {
	/* create a system to solve */
	a = random_matrix(n, 2 * n);

	/* display the matrix */
	if (n <= 10) {
		display_matrix(stdout, a, n, 2 * n);
	}

	/* perform the Gauss algorithm */
	gauss();

	/* display the matrix */
	if (n <= 10) {
		display_matrix(stdout, a, n, 2 * n);
	}

	free(a);
}

/**
 * \brief main function
 */
int	main(int argc, char *argv[]) {
	n = 10;

	// parse the command line
	int	c;
	while (EOF != (c = getopt(argc, argv, "p:")))
		switch (c) {
		case 'p':
			matrix_precision = atoi(optarg);
			break;
		}

	// each subsequent argument is a matrix size for which to perform
	// an experiment and measure run time
	while (optind < argc) {
		n = atoi(argv[optind]);
		if (n <= 0) {
			fprintf(stderr, "not a valid number: %s\n",
				argv[optind]);
		}
		experiment(n);
		optind++;
	}
	
	return EXIT_SUCCESS;
}
