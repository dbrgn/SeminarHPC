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

typedef struct {
	F	*a;
	int	n;
} common_t;
common_t	common;

/**
 * \brief threaded Gauss algorithm implementation
 */
void	gauss() {
	double	start = gettime();
	int	i = 0;
	F	*a = common.a;
	int	n = common.n;
	do {
		F	pivot = M(a, 2 * n, i, i);
		for (int j = i; j < 2 * n; j++) {
			M(a, 2 * n, i, j) /= pivot;
		}
#pragma omp parallel for
		for (int k = 0; k < n; k++) {
			if (k != i) {
				F	b = M(a, 2 * n, k, i);
				for (int j = i; j < 2 * n; j++) {
					M(a, 2 * n, k, j) -= b * M(a, 2 * n, i, j);
				}
			}
		}
		i++;
	} while (i < n);
	double	end = gettime();
	printf("%d,%.6f\n", n, end - start);
}

void	experiment(int n) {
	/* create a system to solve */
	common.n = n;
	common.a = random_matrix(n, 2 * n);

	/* display the matrix */
	if (n <= 10) {
		display_matrix(stdout, common.a, n, 2 * n);
	}

	/* perform the Gauss algorithm */
	gauss();

	/* display the matrix */
	if (n <= 10) {
		display_matrix(stdout, common.a, n, 2 * n);
	}

	free(common.a);
}

int	main(int argc, char *argv[]) {
	int	n = 10;
	int	c;
	while (EOF != (c = getopt(argc, argv, "p:")))
		switch (c) {
		case 'p':
			matrix_precision = atoi(optarg);
			break;
		}

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
