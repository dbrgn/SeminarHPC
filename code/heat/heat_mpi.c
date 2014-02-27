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
	int	dryrun = 0;
	int	steps = 1;

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
	while (EOF != (c = getopt(argc, argv, "h:")))
		switch (c) {
		case 'h':
			h = atof(optarg);
			break;
		case 'r':
			dryrun = 1;
			break;
		case 's':
			steps = atoi(optarg);
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
	if (argc <= optind) {
		fprintf(stderr, "netcdf file name argument missing\n");
		return EXIT_FAILURE;
	}
	char	*netcdffilename = argv[optind++];

	// image file and output file
	heatfile_t      *hf = NULL;
	image_t	*image = NULL;
	int	dimensions[2];
	
	// process zero initializes and writes data
	if (rank == 0) {
		// create the output file
		if (!dryrun) {
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
	int	width = ranges[4 * rank + 1] - ranges[4 * rank + 0] + 2;
	int	height = ranges[4 * rank + 3] - ranges[4 * rank + 2] + 2;
	int	length = width * height;
	double	*u = (double *)malloc(length * sizeof(double));
	double	*unew = (double *)malloc(length * sizeof(double));
	fprintf(stderr, "[%d]: %d bytes allocated\n", rank, length);

	double	start = gettime();

	// process 0 has to send the data to all the other processes
	if (rank == 0) {
		//MPI_Send(buffer, h * n, MPI_FLOAT, r, tag, MPI_COMM_WORLD);
	} else {
		// receive my part of the matrix
		//ierr = MPI_Recv(buffer, height * n, MPI_FLOAT, 0, tag,
		//	MPI_COMM_WORLD, &status);
		//int	count;
		//MPI_Get_count(&status, MPI_FLOAT, &count);
	}
	tag++;

	// start the solver algorithm

		// send the pivot row to all other processes, as a side effect,
		// all processes are synchronized on this point
		//MPI_Bcast(p, 2 * n, MPI_FLOAT, sender, MPI_COMM_WORLD);

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
