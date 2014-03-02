/*
 * heat.c -- solution of heat equation
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdio.h>
#include <stdlib.h>
#include "output.h"
#include <getopt.h>
#include <common.h>

int	debug = 0;

/**
 * \brief Usage function
 *
 * Inform user about options.
 */
void	usage(const char *progname) {
	fprintf(stderr, "usage: %s [ -?r ] [ -n n ] [ -h timestep ] [ -s steps ] [ -t maxtime ] [ -T threads ] netcdffile\n", progname);
	fprintf(stderr, "compute one-dimensional heat equation solution on a unit interval\n");
	fprintf(stderr, "and write results to netcdf file\n");
	fprintf(stderr, "options:\n");
	fprintf(stderr, " -d             increase debug level\n");
	fprintf(stderr, " -h timestep    use different time step, in units of the maximal time step\n");
	fprintf(stderr, " -n n           subdivisions of interval\n");
	fprintf(stderr, " -r             dry run, don't output anything\n");
	fprintf(stderr, " -s steps       record only solutions at a multiple of <steps>\n");
	fprintf(stderr, " -t maxtime     do simulation up to time <maxtime>\n");
	fprintf(stderr, " -T threads     use <threads> threads for iteration step parallelization\n");
	fprintf(stderr, "                default is 1 thread, use carefully\n");
	fprintf(stderr, " -?             display this help message\n");
}

/**
 * \brief main function
 */
int	main(int argc, char *argv[]) {
	double	ht = 1;		// default time step = maximum step
	int	n = 10;		// subdivide into 10 intervals by default
	int	steps = 1;	// record every step
	double	maxt = 1;	// simulate up to time 1
	int	threads = 1;	// default number of threads
	int	dryrun = 0;	// no dry run, write data

	// parse command line
	int	c;
	while (EOF != (c = getopt(argc, argv, "dh:n:rs:t:T:?")))
		switch (c) {
		case 'd':
			debug++;
			break;
		case 'h':
			ht = atof(optarg);
			break;
		case 'n':
			n = atoi(optarg);
			break;
		case 'r':
			dryrun = 1;
			break;
		case 's':
			steps = atoi(optarg);
			break;
		case 't':
			maxt = atof(optarg);
			break;
		case 'T':
			threads = atoi(optarg);
			break;
		case '?':
			usage(argv[0]);
			return EXIT_SUCCESS;
			break;
		}

	// compute step size. For the iteration algorithm to be stable,
	// the value of ht must be smaller than hx^2/4. The ht value set
	// with the -h option is multiplied by this maximum value.
	double	hx = 1. / (n + 1);	// x subdivision depends on n
	ht = ht * hx * hx / 4;
	if (debug) {
		fprintf(stderr, "%s:%d: using time step %f\n",
			__FILE__, __LINE__, ht);
	}

	// next argument must be the file name for the NetCDF file
	if (argc <= optind) {
		fprintf(stderr, "output filename misssing\n");
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	char	*filename = argv[optind];
	heatfile_t	*hf = NULL;
	if (!dryrun) {
		hf = output_create(filename, hx, steps * ht, n);
		if (NULL == hf) {
			fprintf(stderr, "cannot create output file\n");
			return EXIT_FAILURE;
		}
		if (debug) {
			fprintf(stderr, "%s:%d: writing output to %s\n",
				__FILE__, __LINE__, filename);
		}
	}

	// allocate memory for solution. To simplify the formulation of the
	// algorithm, we extend the array of values we want to compute by
	// boundary values, so we allocate arrays of size n+2 instead of 
	// just n. 
	double	*u = (double *)malloc((n + 2) * sizeof(double));
	double	*unew = (double *)malloc((n + 2) * sizeof(double));
	double	*b = (double *)malloc((n + 2) * sizeof(double));

	// compute initial values. The initial function has value 1 in the
	// middle third of the interval, and 0 otherwise
	for (int i = 0; i < n + 2; i++) {
		if ((i < (n + 1) / 3) || (i > 2 * (n + 1) / 3)) {
			u[i] = 0;
		} else {
			u[i] = 1;
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d: initialization complete\n",
			__FILE__, __LINE__);
	}
	if (hf) {
		// write the initial function to the NetCDF file
		output_add(hf, 0, u + 1);
	}

	// get start time for timing measurement
	double	start = gettime();

	// compute the solution using a time marching algorithm and an iterative
	// algorithm to solve the linear system of equations.
	double	t = 0;			// simulation time
	double	hx2 = 2 * hx * hx;	// needed for second derivative
	int	tcounter = 0;		// counts time steps
	while (t < maxt) {
		tcounter++;
		t += ht;

		// compute b array
		for (int j = 1; j <= n; j++) {
			b[j] = -(u[j-1] - 2 * u[j] + u[j+1]) / hx2
				- u[j] / ht;
		}

		// first approximation
		unew[0] = 0;
		unew[n + 1] = 0;
		for (int i = 0; i < n + 2; i++) {
			unew[i] = u[i];
		}

		// now performa iteration 30 times to get the solution
		// of the equation for the next time step
		for (int k = 0; k < 30; k++) {
			// the iteration step can be parallelized as a
			// parallel for, but one has to be very careful about
			// the number of threads or it will not help performance
			// at all
#pragma omp parallel for num_threads(threads)
			for (int j = 1; j <= n; j++) {
				unew[j] = -ht * (b[j] - (u[j-1] - 2 * u[j] + u[j+1]) / hx2);
			}

			// copy new vector to old location
#pragma omp parallel for num_threads(threads)
			for (int j = 0; j < n + 2; j++) {
				u[j] = unew[j];
			}
		}

		// if the result is divisible by steps, we add a row to the
		// output file
		if (0 == tcounter % steps) {
			if (hf) {
				output_add(hf, tcounter / steps, u + 1);
			}
		}
	}

	// report timing
	double	end = gettime();
	printf("%d,%f,%d\n", n, end - start, threads);

	// close the file
	if (hf) {
		output_close(hf);
	}

	// clean up the memory, to ensure Raphael Nestler will not complain ;-)
	free(u);
	free(unew);
	free(b);

	return EXIT_SUCCESS;
}
