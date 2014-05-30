/*
 * boundary.c -- functions related to boundary data interchange
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include "boundary.h"
#include <mpi.h>
#include <stdio.h>

extern int	debug;

/**
 * \brief Send the boundary data 
 *
 * This function is used to send the boundary data to other ranks.
 */
static void	send_boundary(double *data, int size, int partnerrank, int tag,
	MPI_Request *request) {
	if (debug) {
		fprintf(stderr, "%s:%d: send %d values to %d\n",
			__FILE__, __LINE__,  size, partnerrank);
	}
	int	ierr = MPI_Isend(data, size, MPI_DOUBLE, partnerrank, tag,
		MPI_COMM_WORLD, request);
	if (ierr) {
		fprintf(stderr, "cannot send to %d: %d\n", ierr, partnerrank);
		return;
	}
}

/**
 * \brief Send all the boundary data to other processes
 *
 * This function copies the data to the send buffer and call the send_boundary
 * function for each boundary.
 */
static void	send_boundaries(udata_t *u, int tag) {
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: sending boundaries\n",
			__FILE__, __LINE__, u->rank);
	}
	// left
	if (u->rh > 0) {
		for (int i = 0; i < u->height; i++) {
			u->send_left[i] = u->u[i * u->width];
		}
		// exchange with left neighbor
		send_boundary(u->send_left, u->height, u->rank - 1, tag,
			&u->left_request);
	}

	// right
	if (u->rh < u->nx - 1) {
		for (int i = 0; i < u->height; i++) {
			u->send_right[i] = u->u[i * u->width + u->width - 1];
		}
		// exchange with right neighbor
		send_boundary(u->send_right, u->height, u->rank + 1, tag,
			&u->right_request);
	}

	// top
	if (u->rv > 0) {
		for (int j = 0; j < u->width; j++) {
			u->send_top[j] = u->u[j];
		}
		// exchange with top neighbor
		send_boundary(u->send_top, u->width, u->rank - u->nx, tag,
			&u->top_request);
	}

	// bottom
	if (u->rv < u->ny - 1) {
		for (int j = 0; j < u->width; j++) {
			u->send_bottom[j] = u->u[j + u->width * (u->height - 1)];
		}
		// exchange with bottom neighbor
		send_boundary(u->send_bottom, u->width, u->rank + u->nx, tag,
			&u->bottom_request);
	}
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: boundaries sent\n",
			__FILE__, __LINE__, u->rank);
	}
}

/**
 * \brief Receive boundary data from other processes
 *
 * This function receives boundary data into the boundary arrays
 */
static void	recv_boundary(double *data, int size, int partnerrank,
		int tag) {
	if (debug) {
		fprintf(stderr, "%s:%d: receive %d values from %d\n",
			__FILE__, __LINE__, size, partnerrank);
	}
	int	ierr;
	MPI_Status	status;
	ierr = MPI_Recv(data, size, MPI_DOUBLE, partnerrank, tag,
		MPI_COMM_WORLD, &status);
	if (ierr) {
		fprintf(stderr, "%s:%d: could not receive data: %d\n",
			__FILE__, __LINE__, ierr);
	}
	if (debug) {
		fprintf(stderr, "%s:%d: %d values from %d received\n",
			__FILE__, __LINE__, size, partnerrank);
	}
}

/**
 * \brief Receive all the boundary data
 */
static void	recv_boundaries(udata_t *u, int tag) {
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: start boundary exchange\n",
			__FILE__, __LINE__, u->rank);
	}
	// left
	if (u->rh > 0) {
		// exchange with left neighbor
		recv_boundary(u->left, u->height, u->rank - 1, tag);
	}

	// right
	if (u->rh < u->nx - 1) {
		// exchange with right neighbor
		recv_boundary(u->right, u->height, u->rank + 1, tag);
	}

	// top
	if (u->rv > 0) {
		// exchange with top neighbor
		recv_boundary(u->top, u->width, u->rank - u->nx, tag);
	}

	// bottom
	if (u->rv < u->ny - 1) {
		recv_boundary(u->bottom, u->width, u->rank + u->nx, tag);

	}
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: boundary exchange complete\n",
			__FILE__, __LINE__, u->rank);
	}
}

/**
 * \brief Synchronize boundary data with all other processes
 */
void	exchange_boundaries(udata_t *u, int tag) {
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: start boundary exchange\n",
			__FILE__, __LINE__, u->rank);
	}
	send_boundaries(u, tag);
	recv_boundaries(u, tag);

	if (debug) {
		fprintf(stderr, "%s:%d[%d]: boundary exchange complete\n",
			__FILE__, __LINE__, u->rank);
	}
}

