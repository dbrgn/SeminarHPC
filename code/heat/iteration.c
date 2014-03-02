/*
 * iteration.c -- iteration and computation related functions
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include "iteration.h"
#include "domain.h"
#include <stdlib.h>
#include <stdio.h>

extern int	debug;

/**
 * \brief Compute the laplacian
 *
 * This uses the U function to compute the laplacian operator at point (i,j)
 */
static double	laplacian(const udata_t *u, int i, int j) {
	double	l = (-4 * U(u, i, j)
			+ U(u, i - 1, j)
			+ U(u, i + 1, j)
			+ U(u, i,     j - 1)
			+ U(u, i,     j + 1)) / u->h2;
	return l;
}

/**
 * \brief Compute the b vector
 */
void	compute_b(udata_t *u) {
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: computation of b\n",
			__FILE__, __LINE__, u->rank);
	}
	for (int i = 1; i <= u->height; i++) {
		for (int j = 1; j <= u->width; j++) {
			double	b;
			b = -laplacian(u, i, j) - U(u, i, j) / u->ht;
			u->b[j - 1 + (i - 1) * u->width] = b;
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d:[%d]: computation of b complete\n",
			__FILE__, __LINE__, u->rank);
	}
}

/**
 * \brief Compute a single u iteration step
 */
void	iterate_u(double *unew, const udata_t *u) {
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: iteration step for u\n",
			__FILE__, __LINE__, u->rank);
	}
	// perform iteration step
	for (int i = 1; i <= u->height; i++) {
		for (int j = 1; j <= u->width; j++) {
			double	v;
			v = -u->ht * (B(u, i, j) - laplacian(u, i, j));
			unew[j - 1 + (i - 1) * u->width] = v;
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: iteration step for u complete\n",
			__FILE__, __LINE__, u->rank);
	}
}

