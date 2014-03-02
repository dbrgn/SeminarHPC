/*
 * domain.c -- domain related stuff for the MPI implementation
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include "domain.h"
#include <stdlib.h>
#include <stdio.h>
#include "output.h"
#include "image.h"

extern int	debug;

/**
 * \brief initialize a double vector of a given size
 */
double	*doublevector(int size) {
	double	*result = (double *)malloc(size * sizeof(double));
	for (int i = 0; i < size; i++) {
		result[i] = 0;
	}
	return result;
}

/**
 * \brief allocate all data arrays needed for the computation
 */
void	allocate_u(udata_t *u) {
	u->length = u->width * u->height;
	u->u = doublevector(u->length);
	u->b = doublevector(u->length);
	if (debug) {
		fprintf(stderr, "[%d]: %ld bytes allocated\n", u->rank,
			u->length * sizeof(double));
	}

	// allocate memory for the borders from other areas
	u->left = doublevector(u->height);
	u->right = doublevector(u->height);
	u->top = doublevector(u->width);
	u->bottom = doublevector(u->width);

	u->send_left = doublevector(u->height);
	u->send_right = doublevector(u->height);
	u->send_top = doublevector(u->width);
	u->send_bottom = doublevector(u->width);
}

/**
 * \brief free the arrays allocated
 */
void	free_u(udata_t *u) {
	free(u->u);		u->u = NULL;
	free(u->b);		u->b = NULL;

	free(u->left);		u->left = NULL;
	free(u->right);		u->right = NULL;
	free(u->top);		u->top = NULL;
	free(u->bottom);	u->bottom = NULL;

	free(u->send_left);	u->send_left = NULL;
	free(u->send_right);	u->send_right = NULL;
	free(u->send_top);	u->send_top = NULL;
	free(u->send_bottom);	u->send_bottom = NULL;
}

/**
 * \brief Access to the u data
 *
 * This method gives access to the data in the u array, including the special
 * cases when (i,j) is a boundary point.
 */
double	U(const udata_t *u, int i, int j) {
	if (i == 0) {
		if (j == 0) {
			return 0;
		}
		if (j == u->width + 1) {
			return 0;
		}
		return u->top[j - 1];
	}
	if (i == u->height + 1) {
		if (j == 0) {
			return 0;
		}
		if (j == u->width + 1) {
			return 0;
		}
		return u->bottom[j - 1];
	}
	if (j == 0) {
		return u->left[i - 1];
	}
	if (j == u->width + 1) {
		return u->right[i - 1];
	}
	return u->u[(j - 1) + (i - 1) * u->width];
}

/**
 * \brief Access to the b vector
 */
double	B(const udata_t *u, int i, int j) {
	return u->b[j - 1 + (i - 1) * u->width];
}

