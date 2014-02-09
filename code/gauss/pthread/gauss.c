/*
 * gauss.c -- Gauss Algorithmus, konventionelle Implementation
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include "barrier.h"
#include <common.h>

#ifdef DOUBLE
#define	F	double
#define display_matrix	display_double_matrix
#define random_matrix	random_double_matrix
#else
#define	F	float
#define display_matrix	display_float_matrix
#define random_matrix	random_float_matrix
#endif

/**
 * \brief common information, needed in all threads
 */
typedef struct {
	F	*a;	// array
	int	n;	// dimensions
	int	nthreads;
	pthread_barrier_t	barrier1;
	pthread_barrier_t	barrier2;
} common_info;
common_info	common;

/**
 * \brief Information needed within a thread
 */
typedef struct {
	int	min;
	int	max;
	pthread_t	thread;
	float	b;
} thread_info;

thread_info	*info;

// forward declaration of the thread main function
void	*thread_main(void *info);

/**
 * \brief Start a number of threads
 */
void	start_threads(int nthreads) {
	// allocate array with thread information
	common.nthreads = nthreads;
	info = (thread_info *)malloc(nthreads * sizeof(thread_info));
	memset(info, 0, nthreads * sizeof(thread_info));

	// initialize the barrier
	pthread_barrier_init(&common.barrier1, NULL, common.nthreads);
	pthread_barrier_init(&common.barrier2, NULL, common.nthreads);

	// fill the info structure for each thread, including launching
	// the thread
	info[0].min = 0;
	info[nthreads - 1].max = common.n;
	double	step = common.n / (double)nthreads;
	int	i = 0;
	while (i < nthreads) {
		if (i < (nthreads - 1)) {
			info[i].max = i * step;
		}

		// initialize the thread
		pthread_attr_t	attr;
		pthread_attr_init(&attr);
		pthread_create(&info[i].thread, &attr, thread_main, &info[i]);

		// min of next thread
		i++;
		if (i < nthreads) {
			info[i].min = info[i - 1].max;
		}
	}
}

void	join_threads() {
	for (int i = 0; i < common.nthreads; i++) {
		void	*result;
		pthread_join(info[i].thread, &result);
	}
}

/**
 * \brief Thread main function
 */
void	*thread_main(void *arg) {
	// get info about the thread
	thread_info	*this = (thread_info *)arg;

	// start measuring the time
	double	start = gettime();
	int	i = 0; // current pivot row
	F	*a = common.a;
	int	n = common.n;
	do {
		// the first thread das the pivot row operation
		if (pthread_self() == info[0].thread) {
			F	pivot = a[i + 2 * n * i];
			for (int j = i; j < 2 * n; j++) {
				a[j + 2 * n * i] /= pivot;
			}
		}

		// barrier to ensure that the pivot row operation is complete
		pthread_barrier_wait(&common.barrier1);
		F	*a = common.a;
		int	n = common.n;
		for (int k = this->min; k < this->max; k++) {
			if (k != i) {
				this->b = a[i + 2 * n * k];
				for (int j = i; j < 2 * n; j++) {
					a[j + 2 * n * k] -= this->b * a[j + 2 * n * i];
				}
			}
		}

		// barrier to ensure that the row operations have copmleted...
		pthread_barrier_wait(&common.barrier2);

		// ...before the next pivot is started
		i++;
	} while (i < n);

	// let thread 0 display the tiem information
	double	end = gettime();
	if (pthread_self() == info[0].thread) {
		printf("%d,%.6f,%d\n", common.n, end - start, common.nthreads);
	}

	// that's it, return the structure to indicate that there was no
	// problem
	return info;
}

/**
 * \brief threaded Gauss algorithm implementation
 *
 * \param nthreads	number of threads to launch
 */
void	gauss(int nthreads) {
	start_threads(nthreads);
	join_threads();
}

/**
 * \brief perform a Gauss experiment
 *
 * \param n		size of the matrix
 * \param nthreads	number of threads to use during the computation
 */
void	experiment(int n, int nthreads) {
	/* create a system to solve */
	common.n = n;
	common.a = random_matrix(n, 2 * n);

	/* display the matrix */
	if (n <= 10) {
		display_matrix(stdout, common.a, n, 2 * n);
	}

	/* perform the Gauss algorithm */
	gauss(nthreads);

	/* display the matrix */
	if (n <= 10) {
		display_matrix(stdout, common.a, n, 2 * n);
	}

	free(common.a);
}

/**
 * \brief main function
 */
int	main(int argc, char *argv[]) {
	int	n = 10;
	int	nthreads = 1;

	// parse the command line
	int	c;
	while (EOF != (c = getopt(argc, argv, "p:t:")))
		switch (c) {
		case 'p':
			matrix_precision = atoi(optarg);
			break;
		case 't':
			nthreads = atoi(optarg);
			break;
		}

	// each subsequent argument is a to be interpreted as a number giving
	// the dimension of the matrix
	while (optind < argc) {
		n = atoi(argv[optind]);
		if (n <= 0) {
			fprintf(stderr, "not a valid number: %s\n", argv[optind]);
		}
		experiment(n, nthreads);
		optind++;
	}
	
	return EXIT_SUCCESS;
}
