/*
 * gauss.c -- Gauss Algorithmus, konventionelle Implementation
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <common.h>

#ifndef DOUBLE
#define	F	float
#define	display_matrix	display_float_matrix
#define	random_matrix	random_float_matrix
#define unit_matrix	float_unit_matrix
#else
#define	F	double
#define	display_matrix	display_double_matrix
#define	random_matrix	random_double_matrix
#define unit_matrix	double_unit_matrix
#endif

int	unidirectional = 0;

void	gaussstep_both(F *a, int n, int i, int minj) {
	F	pivot = a[i + 2 * n * i];
	for (int j = minj; j < 2 * n; j++) {
		a[j + 2 * n * i] /= pivot;
	}
	for (int k = 0; k < n; k++) {
		if (k != i) {
			F	b = a[i + 2 * n * k];
			for (int j = minj; j <= minj + n; j++) {
				a[j + 2 * n * k] -= b * a[j + 2 * n * i];
			}
		}
	}
}

void	gaussstep_forward(F *a, int n, int i, int minj) {
	F	pivot = M(a, 2 * n, i, i);
	for (int j = minj + 1; j < 2 * n; j++) {
		M(a, 2 * n, i, j) /= pivot;
	}
	for (int k = i + 1; k < n; k++) {
		F	b = M(a, 2 * n, k, i);
		for (int j = minj + 1; j < 2 * n; j++) {
			M(a, 2 * n, k, j) -= b * M(a, 2 * n, i, j);
		}
	}
}

void	gaussstep_backward(F *a, int n, int i, int minj) {
	for (int k = i - 1; k >= 0; k--) {
		F	b = M(a, 2 * n, k, i);
		for (int j = n; j < 2 * n; j++) {
			M(a, 2 * n, k, j) -= b * M(a, 2 * n, i, j);
		}
	}
}

void	gauss(F *a, int n) {
	if (unidirectional) {
		for (int i = 0; i < n; i++) {
			gaussstep_both(a, n, i, i);
		}
	} else {
		for (int i = 0; i < n; i++) {
			gaussstep_forward(a, n, i, i);
		}
		for (int i = n - 1; i >= 0; i--) {
			gaussstep_backward(a, n, i, i);
		}
	}
}

void	experiment(int n) {
	/* create a system to solve */
	F	*a = random_matrix(n, 2 * n);
	F	*L = unit_matrix(n);
	F	*U = unit_matrix(n);

	/* display the matrix */
	if (n <= 10) {
		display_matrix(stdout, a, n, 2 * n);
	}

	/* perform the Gauss algorithm */
	double	start = gettime();
	gauss(a, n);
	double	end = gettime();
	printf("%d, %.6f\n", n, end - start);
	fflush(stdout);

	/* display the matrix */
	if (n <= 10) {
		display_matrix(stdout, a, n, 2 * n);

		if (!unidirectional) {
			// seprate L and U
			for (int i = 0; i < n; i++) {
				for (int j = 0; j <= i; j++) {
					M(L, n, i, j) = M(a, 2 * n, i, j);
				}
			}
			for (int i = 0; i < n; i++) {
				for (int j = i + 1; j < n; j++) {
					M(U, n, i, j) = M(a, 2 * n, i, j);
				}
			}
			display_matrix(stdout, L, n, n);
			display_matrix(stdout, U, n, n);
		}
	}

	free(a);
}

int	main(int argc, char *argv[]) {
	init_gettime();
	int	n = 10;
	int	c;
	while (EOF != (c = getopt(argc, argv, "p:u")))
		switch (c) {
		case 'p':
			matrix_precision = atoi(optarg);
			break;
		case 'u':
			unidirectional = 1;
			break;
		}

	while (optind < argc) {
		n = atoi(argv[optind]);
		if (n <= 0) {
			fprintf(stderr, "not a valid number: %s\n", argv[optind]);
		}
		experiment(n);
		optind++;
	}
	
	return EXIT_SUCCESS;
}
