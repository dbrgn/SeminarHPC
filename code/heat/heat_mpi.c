/*
 * heat.c -- MPI implementation of a solver for the heat equation
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
#include <fitsio.h>
#include "output.h"

typedef struct {
	int	width;
	int	height;
	double	*data;
} image_t;

image_t	*readimage(const char *filename) {
	char	errmsg[80];
	image_t	*image = NULL;
	// open the FITS file
	int	status = 0;
	fitsfile	*fits = NULL;
	if (fits_open_file(&fits, filename, READONLY, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot read fits file %s: %s\n",
			filename, errmsg);
		return NULL;
	}

	// find dimensions of pixel array
	int	igt;
	int	naxis;
	long	naxes[3];
	if (fits_get_img_param(fits, 3, &igt, &naxis, naxes, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot read fits file info: %s\n", errmsg);
		goto bad;
	}
	image = (image_t *)malloc(sizeof(image_t));
	image->width = naxes[0];
	image->height = naxes[1];

	// read the pixel data
	long	npixels = image->width * image->height;
	image->data = (double *)malloc(npixels * sizeof(double));
	long	firstpixel[3] = { 1, 1, 1 };
	if (fits_read_pix(fits, TDOUBLE, firstpixel, npixels, NULL, image->data,
		NULL, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot read pixel data: %s\n", errmsg);
		goto bad;
	}

	// close the file
	if (fits_close_file(fits, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot close fits file %s: %s\n",
			filename, errmsg);
		goto bad;
	}

	// return the image
	return image;
bad:
	if (image) {
		if (image->data) {
			free(image->data);
		}
		free(image);
	}
	if (fits) {
		fits_close_file(fits, &status);
	}
	return NULL;
}

typedef struct {
	double	*u;
	double	*b;
	double	*left;
	double	*right;
	double	*top;
	double	*bottom;
	int	width;
	int	height;
	int	length;
	int	rh;
	int	rv;
} udata_t;

static double	U(const udata_t *u, int i, int j) {
	if (i == 0) {
		if (j == 0) {
			return 0;
		}
		if (j == u->width + 1) {
			return 0;
		}
		return u->top[j - 1];
	}
	if (i == u->height) {
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
	return u->u[i + j * u->width];
}

static double	B(const udata_t *u, int i, int j) {
	return u->b[j - 1 + (i - 1) * u->width];
}

/**
 * \brief main function
 */
int	main(int argc, char *argv[]) {
	int	rank;
	int	ierr;
	int	num_procs;
	MPI_Status	status;
	int	tag = 1;
	double	h = 0.001;
	int	steps = 1;
	double	maxtime = 1;

	// initialize MPI
	ierr = MPI_Init(&argc, &argv);

	// get MPI dimension parameters
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	char	rankprefix[10];
	snprintf(rankprefix, sizeof(rankprefix), "%d", rank);

	// parse the command line
	int	c;
	int	n = 10;
	while (EOF != (c = getopt(argc, argv, "h:r:s:t:")))
		switch (c) {
		case 'h':
			h = atof(optarg);
			break;
		case 's':
			steps = atoi(optarg);
			break;
		case 't':
			maxtime = atof(optarg);
			break;
		}

	double	ht = h * h / 8;

	// next argument is image file name
	if (argc <= optind) {
		fprintf(stderr, "image file name argument missing\n");
		return EXIT_FAILURE;
	}
	char	*imagefilename = argv[optind++];

	// next argument is output file name
	char	*netcdffilename = NULL;
	if (argc <= optind) {
		netcdffilename = argv[optind++];
	}

	// image file and output file
	heatfile_t      *hf = NULL;
	image_t	*image = NULL;
	int	dimensions[2];
	
	// process zero initializes and writes data
	if (rank == 0) {
		// create the output file
		if (netcdffilename) {
			hf = output_create(netcdffilename, h, steps * ht, n);
			if (NULL == hf) {
				fprintf(stderr, "cannot create output file\n");
				return EXIT_FAILURE;
			}
		}

		// read the image file
		image = readimage(imagefilename);
		if (NULL == image) {
			fprintf(stderr, "cannot read image\n");
			return EXIT_FAILURE;
		}
		fprintf(stderr, "%d x %d image read\n", image->width,
			image->height);

		// send the image dimensions to all the other processes
		dimensions[0] = image->width;
		dimensions[1] = image->height;
	}

	// synchronize image dimensions
	MPI_Bcast(dimensions, 2, MPI_INT, 0, MPI_COMM_WORLD);
	fprintf(stderr, "[%d]: %d x %d\n", rank, dimensions[0], dimensions[1]);

	// index ranges for each rank
	int	*ranges = (int *)malloc(4 * num_procs * sizeof(int));
	if (rank == 0) {
		n = sqrt(num_procs);
		double	w = image->width / (double)n;
		double	h = image->height / (double)n;
		for (int r = 0; r < num_procs; r++) {	
			ranges[4 * r + 0] = r * w;
			ranges[4 * r + 1] = image->width;
			ranges[4 * r + 2] = r * h;
			ranges[4 * r + 3] = image->height;
			if (r) {
				ranges[4 * r - 3] = ranges[4 * r + 0];
				ranges[4 * r - 1] = ranges[4 * r + 2];
			}
		}
	}
	MPI_Bcast(ranges, 4 * num_procs, MPI_INT, 0, MPI_COMM_WORLD);
	fprintf(stderr, "[%d]: [%d,%d) x [%d,%d)\n", rank,
		ranges[4 * rank + 0], ranges[4 * rank + 1],
		ranges[4 * rank + 2], ranges[4 * rank + 3]);

	// allocate memory for the area we are responsible for
	udata_t	udata;
	udata.rh = rank % n;
	udata.rv = rank / n;
	udata.width = ranges[4 * rank + 1] - ranges[4 * rank + 0];
	udata.height = ranges[4 * rank + 3] - ranges[4 * rank + 2];
	udata.length = udata.width * udata.height;
	udata.u = (double *)malloc(udata.length * sizeof(double));
	udata.b = (double *)malloc(udata.length * sizeof(double));
	double	*unew = (double *)malloc(udata.length * sizeof(double));
	fprintf(stderr, "[%d]: %d bytes allocated\n", rank, udata.length);

	// allocate memory for the borders from other areas
	udata.left = (double *)malloc(udata.height * sizeof(double));
	memset(udata.left, 0, udata.height * sizeof(double));
	udata.right = (double *)malloc(udata.height * sizeof(double));
	memset(udata.right, 0, udata.height * sizeof(double));
	udata.top = (double *)malloc(udata.width * sizeof(double));
	memset(udata.top, 0, udata.width * sizeof(double));
	udata.bottom = (double *)malloc(udata.width * sizeof(double));
	memset(udata.bottom, 0, udata.width * sizeof(double));

	double	start = gettime();

	// process 0 has to send the data to all the other processes
	if (rank == 0) {
		for (int r = 1; r < n; r++) {
			// allocate a temporary buffer to 
			int	w = ranges[4 * r + 1] - ranges[4 * r + 0];
			int	h = ranges[4 * r + 3] - ranges[4 * r + 2];
			// send all data from rectangle r to the process r
			for (int v = ranges[4*r+2]; v < ranges[4*r+3]; v++) {
				MPI_Send(image->data + v * image->width + ranges[4*r+0],
					w, MPI_DOUBLE, r, tag, MPI_COMM_WORLD);
			}
		}
	} else {
		// receive my part of the matrix
		for (int v = 0; v < udata.height; v++) {
			ierr = MPI_Recv(udata.u + v * udata.width, udata.width,
				MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
		}
	}
	tag++;

	// start the solver algorithm
	double	t = 0;
	double	h2 = 2 * h * h;
	int	tcounter = 0;
	while (t < maxtime) {
		// advance counters
		t += ht;
		tcounter++;

		// copy everything to unew as the initial approximation
		for (int i = 0; i < udata.length; i++) {
			unew[i] = udata.u[i];
		}

		// compute b vector
		for (int i = 1; i <= udata.height; i++) {
			for (int j = 1; j < udata.width; j++) {
				// compute b
				double	b;
				b = (4 * U(&udata, i, j)
					- U(&udata, i - 1, j)
					- U(&udata, i + 1, j)
					- U(&udata, i, j - 1)
					- U(&udata, i, j + 1)) / h2
					- U(&udata, i, j) / ht;
				udata.b[j - 1 + (i - 1) * udata.width] = b;
			}
		}

		// now perform 30 iterations
		for (int k = 0; k < 30; k++) {
			// distribute current values of boundary to neighbors
			tag++;

			// left
			if (udata.rh > 0) {
				for (int i = 0; i < udata.height; i++) {
					udata.left[i] = udata.u[i * udata.width];
				}
				// sent to left neighbor
				MPI_Send(udata.left, udata.height,
					MPI_DOUBLE, rank - 1, tag,
					MPI_COMM_WORLD);

				// read data from left neighbor
				MPI_Recv(udata.left, udata.height,
					MPI_DOUBLE, rank - 1, tag,
					MPI_COMM_WORLD, &status);
			}

			// right
			if (udata.rh < n - 1) {
				for (int i = 0; i < udata.height; i++) {
					udata.right[i] = udata.u[i * udata.width
						+ udata.width - 1];
				}
				// send to right neighbor
				MPI_Send(udata.right, udata.height,
					MPI_DOUBLE, rank + 1, tag,
					MPI_COMM_WORLD);

				// receive data from right neighbor
				MPI_Recv(udata.right, udata.height,
					MPI_DOUBLE, rank + 1, tag,
					MPI_COMM_WORLD, &status);
			}

			// top
			if (udata.rv > 0) {
				for (int j = 0; j < udata.width; j++) {
					udata.top[j] = udata.u[j];
				}
				// sent to top neighbor
				MPI_Send(udata.top, udata.width,
					MPI_DOUBLE, rank - n, tag,
					MPI_COMM_WORLD);

				// receive data from right neighbor
				MPI_Recv(udata.top, udata.width,
					MPI_DOUBLE, rank - n, tag,
					MPI_COMM_WORLD, &status);
			}

			// bottom
			if (udata.rv < n - 1) {
				for (int j = 0; j < udata.width; j++) {
					udata.bottom[j] = udata.u[j
						+ udata.width * (udata.height - 1)];
				}
				// send to bottom neighbor
				MPI_Send(udata.bottom, udata.width,
					MPI_DOUBLE, rank + n, tag,
					MPI_COMM_WORLD);

				// receive data from right neighbor
				MPI_Recv(udata.bottom, udata.width,
					MPI_DOUBLE, rank + n, tag,
					MPI_COMM_WORLD, &status);
			}

			// perform iteration step
			for (int i = 1; i <= udata.height; i++) {
				for (int j = 1; j <= udata.width; j++) {
					double	v;
					v = ht * (B(&udata, i, j)
						- (4 * U(&udata, i, j)
						- U(&udata, i - 1, j)
						- U(&udata, i + 1, j)
						- U(&udata, i, j - 1)
						- U(&udata, i, j + 1)) / h2);
					unew[j - 1 + (i - 1) * udata.width] = v;
				}
			}
		}

		// copy the new u to the old u
		for (int i = 0; i < udata.length; i++) {
			udata.u[i] = unew[i];
		}

		// decide whether we have to output something
		if (0 == (tcounter % steps)) {
			// output needed
		}
	}

	// the computation is now complete, so rank zero has to collect all
	// the pieces
	if (rank == 0) {
		// receive remaining data from other processes
		//	MPI_Recv(buffer, h * n, MPI_FLOAT, r, tag,
		//		MPI_COMM_WORLD, &status);
	} else {
		// send the data to process 0
		//ierr = MPI_Send(buffer, height * n, MPI_FLOAT, 0, tag,
		//	MPI_COMM_WORLD);
	}

	// measure end time
	double	end = gettime();

	// we are now done, process 0 displays the result
	if (rank == 0) {
		printf("%d,%.6f,%d\n", n, end - start, num_procs);
	}

	// cleanup MPI
	MPI_Finalize();

	return EXIT_SUCCESS;
}
