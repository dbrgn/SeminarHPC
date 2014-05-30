/*
 * partition.c -- partition image into domains
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include "partition.h"
#include "image.h"
#include "domain.h"
#include "iteration.h"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

extern int	debug;

/**
 * \brief Partition the domain
 *
 * This function computes the ranges of indices of the original image
 * this each rank is responsible for.
 */
void	partitiondomain(udata_t *u, const image_t *image) {
	double	w = image->width / (double)u->nx;
	double	h = image->height / (double)u->ny;
	int	num_procs = u->nx * u->ny;
	for (int r = 0; r < num_procs; r++) {	
		int	rx = r % u->nx;
		int	ry = r / u->nx;
		u->ranges[4 * r + 0] = rx * w;
		u->ranges[4 * r + 1] = (rx + 1) * w;
		u->ranges[4 * r + 2] = ry * h;
		u->ranges[4 * r + 3] = (ry + 1) * h;
		if (debug) {
			fprintf(stderr, "%s:%d[%d]: range for "
				"rank %d = (%d,%d): [%d,%d) x [%d,%d)\n",
				__FILE__, __LINE__, u->rank,
				r, rx, ry,
				u->ranges[4 * r + 0],
				u->ranges[4 * r + 1],
				u->ranges[4 * r + 2],
				u->ranges[4 * r + 3]);
		}
	}
}

/**
 * \brief Send image data to a rank != 0 process
 *
 * This functions sends the data for a given rank found in the image to
 * that rank. The range of data to send is found in the udata structure
 * handed in as the first argument.
 */
void	sendimagerange(const udata_t *u, const image_t *image,
	int rank, int tag) {
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: sending image to %d\n",
			__FILE__, __LINE__, u->rank, rank);
	}
	// allocate a temporary buffer to 
	int	width = u->ranges[4 * rank + 1] - u->ranges[4 * rank + 0];
	int	fromrow = u->ranges[4 * rank + 2];
	int	torow = u->ranges[4 * rank + 3];
	int	offset = u->ranges[4 * rank + 0];
	// send all data from rectangle r to the process r
	for (int row = fromrow; row < torow; row++) {
		int	ierr;
		if (debug) {
			fprintf(stderr, "%s:%d[%d]: sending row %d to %d\n",
				__FILE__, __LINE__, u->rank, row, rank);
		}
		ierr = MPI_Send(image->data + row * image->width + offset,
			width, MPI_DOUBLE, rank, tag, MPI_COMM_WORLD);
		if (ierr) {
			fprintf(stderr, "%s:%d[%d]: send error: %d\n",
				__FILE__, __LINE__, u->rank, ierr);
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: sending image to %d complete\n",
			__FILE__, __LINE__, u->rank, rank);
	}
}

/**
 * \brief Receive image data from a different rank
 *
 * This function is used by rank 0 to receive image data from a different rank
 * and integrate it into the image.
 */
void	receiveimagerange(const udata_t *u, image_t *image, int rank,
	int tag) {
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: receiving image from %d\n",
			__FILE__, __LINE__, u->rank, rank);
	}
	// dimensions and parameters of data we want to receive
	int	width = u->ranges[4 * rank + 1] - u->ranges[4 * rank + 0];
	int	fromrow = u->ranges[4 * rank + 2];
	int	torow = u->ranges[4 * rank + 3];
	int	offset = u->ranges[4 * rank + 0];
	// send all data from rectangle r to the process r
	for (int row = fromrow; row < torow; row++) {
		int	ierr;
		MPI_Status	status;
		if (debug) {
			fprintf(stderr, "%s:%d[%d]: receive row %d from %d\n",
				__FILE__, __LINE__, u->rank, row, rank);
		}
		ierr = MPI_Recv(image->data + row * image->width + offset,
			width, MPI_DOUBLE, rank, tag, MPI_COMM_WORLD, &status);
		if (ierr) {
			fprintf(stderr, "%s:%d[%d]: receive error: %d\n",
				__FILE__, __LINE__, u->rank, ierr);
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: image from %d received\n",
			__FILE__, __LINE__, u->rank, rank);
	}
}

/**
 * \brief Copy image data 
 *
 * This function is used by rank 0 to copy the u array, where computation
 * is performed to the image data structure, which is used for output.
 */
void	copytoimage(const udata_t *u, image_t *image) {
	if (u->rank != 0) {
		return;
	}
	int	width = u->ranges[1];
	int	fromrow = u->ranges[2];
	int	torow = u->ranges[3];
	for (int row = fromrow; row < torow; row++) {
		memcpy(image->data + row * image->width, u->u + row * width,
			width * sizeof(double));
	}
}

/**
 * \brief Copy image data to image
 *
 * This function is used by rank 0 to copy image data to the u array
 */
void	copyfromimage(udata_t *u, const image_t *image){
	if (u->rank != 0) {
		return;
	}
	int	width = u->ranges[1];
	int	fromrow = u->ranges[2];
	int	torow = u->ranges[3];
	for (int row = fromrow; row < torow; row++) {
		memcpy(u->u + row * width, image->data + row * image->width,
			width * sizeof(double));
	}
}

/**
 * \brief Receive initial data from rank 0
 *
 * This function is used by ranks different from 0 to receive image data
 * and stores it in the u->u data array.
 */
void	receiverange(udata_t *u, int tag) {
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: receiving range %d\n",
			__FILE__, __LINE__, u->rank, u->rank);
	}
	MPI_Status	status;
	for (int row = 0; row < u->height; row++) {
		if (debug) {
			fprintf(stderr, "%s:%d[%d]: receive row %d\n",
				__FILE__, __LINE__, u->rank, row);
		}
		int	ierr;
		ierr = MPI_Recv(u->u + row * u->width, u->width,
			MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
		if (ierr) {
			fprintf(stderr, "%s:%d[%d]: receive error: %d\n",
				__FILE__, __LINE__, u->rank, ierr);
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: range %d received\n",
			__FILE__, __LINE__, u->rank, u->rank);
	}
}

/**
 * \brief Send data range to rank 0
 *
 * This method is used by the ranks different from 0 to send computed
 * data back to rank 0, where it can be stored in the image array.
 */
void	sendrange(const udata_t *u, int tag) {
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: send range\n",
			__FILE__, __LINE__, u->rank);
	}
	for (int row = 0; row < u->height; row++) {
		if (debug) {
			fprintf(stderr, "%s:%d[%d]: receive row %d\n",
				__FILE__, __LINE__, u->rank, row);
		}
		int	ierr;
		ierr = MPI_Send(u->u + row * u->width, u->width,
			MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
		if (ierr) {
			fprintf(stderr, "%s:%d[%d]: receive error: %d\n",
				__FILE__, __LINE__, u->rank, ierr);
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: range %d sent\n",
			__FILE__, __LINE__, u->rank, u->rank);
	}
}

/**
 * \brief Synchronize image data
 *
 * This function is called by all processes and ensures computed data is
 * sent to rank 0 and integrated into the image.
 */
void	synchronize_image(const udata_t *u, image_t *image, int tag) {
	if (u->rank == 0) {
		// copy data from all other ranks
		int	num_procs = u->nx * u->ny;
		for (int r = 1; r < num_procs; r++) {
			receiveimagerange(u, image, r, tag);
		}
		// copy our own rank 0 data to the image
		copytoimage(u, image);
	} else {
		sendrange(u, tag);
	}
}

