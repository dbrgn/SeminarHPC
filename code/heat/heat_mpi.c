/*
 * heat_mpi.c -- MPI implementation of a solver for the heat equation
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
#include <getopt.h>
#include <common.h>
#include "output.h"
#include "image.h"
#include "domain.h"
#include "iteration.h"
#include "partition.h"

int	debug = 0;

#if 0
/**
 * \brief Partition the domain
 *
 * This function computes the ranges of indices of the original image
 * this each rank is responsible for.
 */
static void	partitiondomain(udata_t *u, const image_t *image) {
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
static void	sendimagerange(const udata_t *u, const image_t *image,
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
static void	receiveimagerange(const udata_t *u, image_t *image, int rank,
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
static void	copytoimage(const udata_t *u, image_t *image) {
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
static void	copyfromimage(udata_t *u, const image_t *image){
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
static void	receiverange(udata_t *u, int tag) {
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
static void	sendrange(const udata_t *u, int tag) {
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
#endif 

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
static void	exchange_boundaries(udata_t *u, int tag) {
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

#if 0
/**
 * \brief Synchronize image data
 *
 * This function is called by all processes and ensures computed data is
 * sent to rank 0 and integrated into the image.
 */
static void	synchronize_image(const udata_t *u, image_t *image, int tag) {
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
#endif

/**
 * \brief main function
 */
int	main(int argc, char *argv[]) {
	int	ierr;
	int	num_procs;
	int	tag = 1;
	double	h = 1;
	int	steps = 1;
	double	maxtime = 1;
	char	*basedir = NULL;

	udata_t	udata;
	udata.nx = 1;
	udata.ny = 1;

	// initialize MPI
	ierr = MPI_Init(&argc, &argv);
	if (ierr) {
		fprintf(stderr, "cannot initialize MPI: %d\n", ierr);
		return EXIT_FAILURE;
	}

	// get MPI dimension parameters
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &udata.rank);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	char	rankprefix[10];
	snprintf(rankprefix, sizeof(rankprefix), "%d", udata.rank);

	if (debug) {
		fprintf(stderr, "%s:%d[%d]: process id %d\n",
			__FILE__, __LINE__, udata.rank, getpid());
	}

	// parse the command line
	int	c;
	while (EOF != (c = getopt(argc, argv, "dh:r:s:t:x:y:b:")))
		switch (c) {
		case 'd':
			debug++;
			break;
		case 'h':
			h = atof(optarg);
			break;
		case 's':
			steps = atoi(optarg);
			break;
		case 't':
			maxtime = atof(optarg);
			break;
		case 'x':
			udata.nx = atoi(optarg);
			break;
		case 'y':
			udata.ny = atoi(optarg);
			break;
		case 'b':
			basedir = optarg;
			break;
		}

	udata.ht = h * h / 8;
	udata.h2 = 2 * h * h;

	// make sure the arguments are consistent
	if (num_procs != udata.nx * udata.ny) {
		fprintf(stderr, "number of processes does not match "
			"dimensions: %d != %d x %d\n", num_procs, udata.nx,
			udata.ny);
		return EXIT_FAILURE;
	}

	// compute horizontal and vertical index
	udata.rh = udata.rank % udata.nx;
	udata.rv = udata.rank / udata.nx;
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: rh = %d, rv = %d\n",
			__FILE__, __LINE__, udata.rank, udata.rh, udata.rv);
	}

	// next argument is image file name
	if (argc <= optind) {
		fprintf(stderr, "image file name argument missing\n");
		return EXIT_FAILURE;
	}
	char	*imagefilename = argv[optind++];

	// next argument is output file name
	char	*netcdffilename = NULL;
	if (argc > optind) {
		netcdffilename = argv[optind++];
		if (debug) {
			fprintf(stderr, "%s:%d: netcdffilename: %s\n",
				__FILE__, __LINE__, netcdffilename);
		}
	}

	// image file and output file
	heatfile_t      *hf = NULL;
	image_t	*image = NULL;
	int	dimensions[2];
	
	// process zero initializes and writes data
	if (udata.rank == 0) {
		// read the image file
		image = readimage(imagefilename);
		if (NULL == image) {
			fprintf(stderr, "cannot read image\n");
			return EXIT_FAILURE;
		}
		if (debug) {
			fprintf(stderr, "%s:%d[%d]: %d x %d image read\n",
				__FILE__, __LINE__, udata.rank,
				image->width, image->height);
		}

		// create the output file
		if (netcdffilename) {
			if (debug) {
				fprintf(stderr, "%s:%d: creating NetCDF %s\n",
					__FILE__, __LINE__, netcdffilename);
			}
			hf = output2_create(netcdffilename, h,
				steps * udata.ht, image->width, image->height);
			if (NULL == hf) {
				fprintf(stderr, "cannot create output file\n");
				return EXIT_FAILURE;
			}
		}

		// send the image dimensions to all the other processes
		dimensions[0] = image->width;
		dimensions[1] = image->height;
	}

	// write the first image
	if ((basedir) && (udata.rank == 0)) {
		char	outfilename[1024];
		snprintf(outfilename, sizeof(outfilename), "%s/00000.fits",
			basedir);
		writeimage(image, outfilename);
	}

	// synchronize image dimensions
	MPI_Bcast(dimensions, 2, MPI_INT, 0, MPI_COMM_WORLD);
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: %d x %d\n",
			__FILE__, __LINE__, udata.rank,
			dimensions[0], dimensions[1]);
	}

	// index ranges for each rank
	udata.ranges = (int *)malloc(4 * num_procs * sizeof(int));
	if (udata.rank == 0) {
		partitiondomain(&udata, image);
	}
	MPI_Bcast(udata.ranges, 4 * num_procs, MPI_INT, 0, MPI_COMM_WORLD);
	int	*range = &udata.ranges[4 * udata.rank];
	if (debug) {
		fprintf(stderr, "%s:%d:[%d]: [%d,%d) x [%d,%d)\n",
			__FILE__, __LINE__, udata.rank,
			range[0], range[1], range[2], range[3]);
	}

	// allocate memory for the area we are responsible for
	udata.width = range[1] - range[0];
	udata.height = range[3] - range[2];
	allocate_u(&udata);
	double	*unew = (double *)malloc(udata.length * sizeof(double));
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: arrays allocated, %d x %d\n",
			__FILE__, __LINE__,
			udata.rank, udata.width, udata.height);
	}

	// write initial data to the output file
	if ((hf) && (udata.rank == 0)) {
		output2_add(hf, 0, image->data);
	}

	// measure start time (after all allocations are done)
	double	start = gettime();

	// process 0 has to send the data to all the other processes
	if (udata.rank == 0) {
		for (int r = 1; r < num_procs; r++) {
			sendimagerange(&udata, image, r, tag);
		}
		copyfromimage(&udata, image);
	} else {
		// receive my part of the matrix
		receiverange(&udata, tag);
	}
	tag++;

	// start the solver algorithm
	double	t = 0;
	int	tcounter = 0;
	while (t < maxtime) {
		// advance counters
		t += udata.ht;
		tcounter++;

		// compute b vector
		compute_b(&udata);

		// copy everything to unew as the initial approximation
		for (int i = 0; i < udata.length; i++) {
			unew[i] = udata.u[i];
		}

		// now perform 30 Jordan iterations
		for (int k = 0; k < 30; k++) {
			// synchronize current values of boundary with neighbors
			tag++;
			exchange_boundaries(&udata, tag);

			// perform iteration step
			iterate_u(unew, &udata);

			// copy the new u to the old u
			for (int i = 0; i < udata.length; i++) {
				udata.u[i] = unew[i];
			}
		}

		// decide whether we have to output something
		if (0 == tcounter % steps) {
			// time value for this data output
			int	tvalue = tcounter / steps;

			// output needed, so we synchronize image data
			tag++;
			synchronize_image(&udata, image, tag);

			// write an image
			if ((basedir) && (udata.rank == 0)) {
				char	outfilename[1024];
				snprintf(outfilename, sizeof(outfilename),
					"%s/%05d.fits", basedir, tvalue);
				writeimage(image, outfilename);
			}

			// write solution data
			if ((hf) && (udata.rank == 0)) {
				output2_add(hf, tvalue, image->data);
			}
		}
	}

	// measure end time
	double	end = gettime();

	// we are now done, process 0 displays the result
	if (udata.rank == 0) {
		printf("%d,%.6f,%d\n", image->width * image->height,
			end - start, num_procs);
	}

	// close the netcdf file
	if ((udata.rank == 0) && (hf)) {
		output_close(hf);
	}

	// cleanup MPI
	MPI_Finalize();

	return EXIT_SUCCESS;
}
