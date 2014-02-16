/*
 * common.c -- common functions 
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include "common.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

float	*random_float_matrix(int n, int m) {
	float	*a = (float *)malloc(n * m * sizeof(float));
	if (NULL == a) {
		fprintf(stderr, "cannot allocate %d x %d array: %s\n",
			n, m, strerror(errno));
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; (j < n) && (j < m); j++) {
			M(a, m, i, j) = random() / (float)0x7fffffff;
		}
		for (int j = n; j < m; j++) {
			M(a, m, i, j) = (i == (j - n)) ? 1 : 0;
		}
	}
	return a;
}

double	*random_double_matrix(int n, int m) {
	double	*a = (double *)malloc(n * m * sizeof(double));
	if (NULL == a) {
		fprintf(stderr, "cannot allocate %d x %d array: %s\n",
			n, m, strerror(errno));
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; (j < n) && (j < m); j++) {
			M(a, m, i, j) = random() / (float)0x7fffffff;
		}
		for (int j = n; j < m; j++) {
			M(a, m, i, j) = (i == (j - n)) ? 1 : 0;
		}
	}
	return a;
}

float	*float_unit_matrix(int n) {
	float	*a = (float *)malloc(n * n * sizeof(float));
	if (NULL == a) {
		fprintf(stderr, "cannot allocate %d x %d array: %s\n",
			n, n, strerror(errno));
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			M(a, n, i, j) = (i == j) ? 1 : 0;
		}
	}
	return a;
}

double	*double_unit_matrix(int n) {
	double	*a = (double *)malloc(n * n * sizeof(double));
	if (NULL == a) {
		fprintf(stderr, "cannot allocate %d x %d array: %s\n",
			n, n, strerror(errno));
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			M(a, n, i, j) = (i == j) ? 1 : 0;
		}
	}
	return a;
}

char	*matrix_prefix = NULL;
int	matrix_precision = 3;

void	display_float_matrix(FILE *f, float *a, int height, int width) {
	if (matrix_prefix) {
		fprintf(f, "%s:[\n", matrix_prefix);
	} else {
		fprintf(f, "[\n");
	}
	for (int i = 0; i < height; i++) {
		if (matrix_prefix) {
			fprintf(f, "%s:", matrix_prefix);
		}
		for (int j = 0; j < width; j++) {
			if (j) { printf(","); }
			fprintf(f, "%*.*f", matrix_precision + 3,
				matrix_precision, M(a, width, i, j));
		}
		fprintf(f, ";\n");
	}
	if (matrix_prefix) {
		fprintf(f, "%s:]\n", matrix_prefix);
	} else {
		fprintf(f, "]\n");
	}
}

void	display_double_matrix(FILE *f, double *a, int height, int width) {
	if (matrix_prefix) {
		fprintf(f, "%s:[\n", matrix_prefix);
	} else {
		fprintf(f, "[\n");
	}
	for (int i = 0; i < height; i++) {
		if (matrix_prefix) {
			fprintf(f, "%s:", matrix_prefix);
		}
		for (int j = 0; j < width; j++) {
			if (j) { printf(","); }
			fprintf(f, "%*.*f", matrix_precision + 3,
				matrix_precision, M(a, width, i, j));
		}
		fprintf(f, ";\n");
	}
	if (matrix_prefix) {
		fprintf(f, "%s:]\n", matrix_prefix);
	} else {
		fprintf(f, "]\n");
	}
}

static time_t	time_origin;

void	init_gettime() {
	time(&time_origin);
}

double	gettime() {
	struct timeval	tv;
	gettimeofday(&tv, NULL);
	double	result = (tv.tv_sec - time_origin) + (0.000001 * tv.tv_usec);
	return result;
}

