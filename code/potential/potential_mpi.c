/*
 * potential_mpi.c -- MPI implementation of a solver for the potential equation
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschulesd Rapperswil
 * (c) 2014 changed from heat eaquation to potential equation by Reto Christen, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <getopt.h>
#include <common.h>
#include "output.h"
#include "image.h"
#include "domain.h"
#include "iteration.h"
#include "partition.h"
#include "boundary.h"

int	debug = 0;

/**
 * \brief usage function
 *
 * Tell user about options and command line arguments
 */
static void	usage(const char *progname) {
	fprintf(stderr, "usage: mpirun -n <n> %s [ -d? ] [ -b basedir ] [ -h h ] [ -s steps ] [ -t maxtime ] [ -x nx ] [ -y ny ] imagefile [ netcdffile ]\n", progname);
	fprintf(stderr, "Solve heat equation for initial condition from <imagefile>\n");
	fprintf(stderr, "and write results to <netcdffile>.\n");
	fprintf(stderr, "options:\n");
	fprintf(stderr, " -b basedir    write images to <basedir> (default: don't write images)\n");
	fprintf(stderr, " -d            increase debug level\n");
	fprintf(stderr, " -s steps      write data/image every <steps> steps (default 1)\n");
	fprintf(stderr, " -t maxsteps   maximum steps (default equal to dimension)\n");
	fprintf(stderr, " -x nx         number of patches in x direction (default 1)\n");
	fprintf(stderr, " -y ny         number of patches in y direction (default 1)\n");
	fprintf(stderr, " -n dimension	sets dimension (default 16)\n");
	fprintf(stderr, " -a algorithm	defines algorithm: Jacobi (0), Gauss-Seidel (1), symmetric Gauss-Seidel (2)\n");
	fprintf(stderr, "This is a MPI-programm, it cannot be run standalone. Run it using mpirun,\n");
	fprintf(stderr, "as shown above. You must start <n> = <nx> x <ny> processes.\n");
}

/**
 * \brief main function
 */
int	main(int argc, char *argv[]) {
	int	ierr;
	int	num_procs;
	int	tag = 1;
	double	h = 1;
	int	steps = 1;
	char	*basedir = NULL;

	udata_t	udata;
	udata.nx = 1;
	udata.ny = 1;
	udata.dimension = 16;
	udata.algorithm = 0;
	udata.picturesteps = 1;

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
	while (EOF != (c = getopt(argc, argv, "b:dh:r:s:t:x:y:n:a:?")))
		switch (c) {
		case 'd':
			debug++;
			break;
		case 's':
			udata.picturesteps = atoi(optarg);
			break;
		case 't':
			udata.maxsteps = atof(optarg);
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
		case 'n':
			udata.dimension = atoi(optarg);
			break;
		case 'a':
			udata.algorithm = atoi(optarg);
			break;
		case '?':
			usage(argv[0]);
			return EXIT_SUCCESS;
		}


	udata.maxsteps = udata.dimension + udata.dimension;
	udata.h = 1 / udata.dimension;

	// make sure the arguments are consistent
	if (num_procs != udata.nx * udata.ny) {
		fprintf(stderr, "number of processes does not match "
			"dimensions: %d != %d x %d\n", num_procs, udata.nx,
			udata.ny);
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	// compute horizontal and vertical index of this rank
	udata.rh = udata.rank % udata.nx;
	udata.rv = udata.rank / udata.nx;
	if (debug) {
		fprintf(stderr, "%s:%d[%d]: rh = %d, rv = %d\n",
			__FILE__, __LINE__, udata.rank, udata.rh, udata.rv);
	}

	// next argument is image file name
	if (argc <= optind) {
		fprintf(stderr, "image file name argument missing\n");
		usage(argv[0]);
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
	}

	// write the first image
	if ((basedir) && (udata.rank == 0)) {
		char	outfilename[1024];
		snprintf(outfilename, sizeof(outfilename), "%s/00000.fits",
			basedir);
		writeimage(image, outfilename);
	}

	// index ranges for each rank
	udata.ranges = (int *)malloc(4 * num_procs * sizeof(int));
	if (udata.rank == 0) {
		partitiondomain(&udata, image);
	}

	// exchange range size information with all other ranks. The ranks
	// then pick the dimensions they need from the array, this is
	// the purpose of the range pointer
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

	// make sure dimension is correct
	if (udata.dimension != udata.height + udata.width) {
		fprintf(stderr, "dimension does not match\n");
		return EXIT_FAILURE;
	}

	// start the solver algorithm
	int	stepcounter = 0;	// counter for time steps
	while (stepcounter < udata.maxsteps) {
		stepcounter++;

		// copy everything to unew as the initial approximation
		for (int i = 0; i < udata.length; i++) {
			unew[i] = udata.u[i];
		}

		// now perform <picturesteps> iterations
		for (int k = 0; k < udata.picturesteps; k++) {
			// synchronize current values of boundary with neighbors
			tag++;
			exchange_boundaries(&udata, tag);

			// perform iteration step
			iterate_u(unew, &udata);

			// copy the new u to the old u / only used for Jacobi
			if (udata.algorithm == 0) {
				for (int i = 0; i < udata.length; i++) {
					udata.u[i] = unew[i];
				}
			}
		}

		// decide whether we have to output something
		if (0) {
			// time value for this data output
			int	stepvalue = stepcounter / udata.picturesteps;

			// output needed, so we synchronize image data
			tag++;
			synchronize_image(&udata, image, tag);

			// write an image
			if ((basedir) && (udata.rank == 0)) {
				char	outfilename[1024];
				snprintf(outfilename, sizeof(outfilename),
					"%s/%05d.fits", basedir, stepvalue);
				writeimage(image, outfilename);
			}

			// write solution data
			if ((hf) && (udata.rank == 0)) {
				output2_add(hf, stepvalue, image->data);
			}
		}
	}

	// measure end time
	double	end = gettime();

	// we are now done, rank 0 displays the result
	if (udata.rank == 0) {
		printf("%.6f",end - start);
	}

	// close the netcdf file
	if ((udata.rank == 0) && (hf)) {
		output_close(hf);
	}

	// cleanup MPI
	MPI_Finalize();

	// cleanup the memory we have allocated
	free(udata.ranges); udata.ranges = NULL;
	free_u(&udata);

	return EXIT_SUCCESS;
}
