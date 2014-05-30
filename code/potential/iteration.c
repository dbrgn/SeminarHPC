/*
 * iteration.c -- iteration and computation related functions
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 * (c) 2014 changed from heat equation to potential equation by Reto Christen, Hochschule Rapperswil
 */
#include "iteration.h"
#include "domain.h"
#include <stdlib.h>
#include <stdio.h>

extern int	debug;
udata_t	udata;

/**
 * \brief Compute the laplacian
 *
 * This uses the U function to compute the laplacian operator at point (i,j)
 */
static double	laplacian(const udata_t *u, int i, int j) {
	double l = (U(u, i - 1, j)
			  + U(u, i + 1, j)
			  + U(u, i,     j - 1)
			  + U(u, i,     j + 1)
			  - (u->h * u->h)) / 4;
	return l;
}


/**
 * \brief Compute a single u iteration step
 */
void	iterate_u(double *unew, const udata_t *u) {
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: iteration step for u\n",
			__FILE__, __LINE__, u->rank);
	}

	// static counter for symmetric Gauss-Seidel
	static int counter = 0;

	// perform iteration step for Jacobi, Gauss-Seidel and symmetric Gauss-Seidel (even step)
	if (u->algorithm == 0 || u->algorithm == 1 || (u->algorithm == 2 && counter % 2 == 0)) {
		for (int i = 1; i <= u->height; i++) {
			for (int j = 1; j <= u->width; j++) {
				double	v;
				// set boarders to 0
				if (u->rank == 0 && i == 1)
					v = 0.0;
				else if (u->rank == 0 && j == 1)
							v = 0.0;
				else if (u->rank == 1 && i == 1)
							v = 0.0;
				else if (u->rank == 1 && j == u->width)
							v = 0.0;
				else if (u->rank == 2 && i == u->height)
							v = 0.0;
				else if (u->rank == 2 && j == 1)
							v = 0.0;
				else if (u->rank == 3 && i == u->height)
							v = 0.0;
				else if (u->rank == 3 && j == u->width)
							v = 0.0;
				else {
					v = laplacian(u, i, j);
				}
				unew[j - 1 + (i - 1) * u->width] = v;

				// copy the new u to the old u / only used for Gauss Seidel and symmetric Gauss-Seidel
				if (u->algorithm == 1 || u->algorithm == 2) {
						u->u[j - 1 + (i - 1) * u->width] = unew[j - 1 + (i - 1) * u->width];
					}
			}
		}
	}

	// perform iteration step for symmetric Gauss-Seidel (uneven step)
	if (u->algorithm == 2 && counter % 2 != 0)	{
		for (int i = u->height; i >= 1; i--) {
				for (int j = u->width; j >= 1; j--) {
					double	v;
					// set boarders to 0
					if (u->rank == 0 && i == 1)
						v = 0.0;
					else if (u->rank == 0 && j == 1)
								v = 0.0;
					else if (u->rank == 1 && i == 1)
								v = 0.0;
					else if (u->rank == 1 && j == u->width)
								v = 0.0;
					else if (u->rank == 2 && i == u->height)
								v = 0.0;
					else if (u->rank == 2 && j == 1)
								v = 0.0;
					else if (u->rank == 3 && i == u->height)
								v = 0.0;
					else if (u->rank == 3 && j == u->width)
								v = 0.0;
					else {
						v = laplacian(u, i, j);
					}
					unew[j - 1 + (i - 1) * u->width] = v;

					// copy the new u to the old u 
					if (u->algorithm == 1 || u->algorithm == 2) {
							u->u[j - 1 + (i - 1) * u->width] = unew[j - 1 + (i - 1) * u->width];
						}
				}
			}
	}

	// increment counter for symmetric Gauss-Seidel
	if (u->algorithm == 2)
		counter++;


	if (debug) {
		fprintf(stderr, "%s:%d[%d]: iteration step for u complete\n",
			__FILE__, __LINE__, u->rank);
	}
}



