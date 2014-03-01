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

int	main(int argc, char *argv[]) {
	double	ht = 1;
	int	n = 10;
	int	steps = 1;
	double	maxt = 1;
	int	threads = 1;
	int	dryrun = 0;

	// parse command line
	int	c;
	while (EOF != (c = getopt(argc, argv, "n:h:s:t:T:r")))
		switch (c) {
		case 'n':
			n = atoi(optarg);
			break;
		case 'h':
			ht = atof(optarg);
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
		case 'r':
			dryrun = 1;
			break;
		}

	// compute step size
	double	hx = 1. / (n + 1);
	ht = ht * hx * hx / 4;

	// next argument must be the file name
	if (argc <= optind) {
		fprintf(stderr, "output filename misssing\n");
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
	}

	// allocate memory for solution
	double	*u = (double *)malloc((n + 2) * sizeof(double));
	double	*unew = (double *)malloc((n + 2) * sizeof(double));
	double	*b = (double *)malloc((n + 2) * sizeof(double));

	// compute initial values
	for (int i = 0; i < n + 2; i++) {
		if ((i < (n + 1) / 3) || (i > 2 * (n + 1) / 3)) {
			u[i] = 0;
		} else {
			u[i] = 1;
		}
	}
	if (hf) {
		output_add(hf, 0, u + 1);
	}

	// timing
	double	start = gettime();

	// compute the solution
	double	t = 0;
	double	hx2 = 2 * hx * hx;
	int	tcounter = 0;
	while (t < maxt) {
		tcounter++;
		t += ht;

		// compute b array
		for (int j = 1; j <= n; j++) {
			b[j] = -(u[j-1] - 2 * u[j] + u[j+1]) / hx2
				- u[j] / ht;
		}

		// perform iterative solution

		// first approximation
		unew[0] = 0;
		unew[n + 1] = 0;
		for (int i = 0; i < n + 2; i++) {
			unew[i] = u[i];
		}

		for (int k = 0; k < 30; k++) {
			// iteration step
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

		// if the result is divisible by steps, we add a row
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

	return EXIT_SUCCESS;
}
