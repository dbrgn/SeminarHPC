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

typedef struct {
	F	*a;	// array
	int	n;	// dimensions
	int	nthreads;
	pthread_barrier_t	barrier1;
	pthread_barrier_t	barrier2;
} common_info;
common_info	common;

typedef struct {
	int	min;
	int	max;
	pthread_t	thread;
} thread_info;

thread_info	*info;

void	*thread_main(void *info);

void	start_threads(int nthreads) {
	common.nthreads = nthreads;
	info = (thread_info *)malloc(nthreads * sizeof(thread_info));
	memset(info, 0, nthreads * sizeof(thread_info));

	// initialize the barrier
	pthread_barrier_init(&common.barrier1, NULL, common.nthreads);
	pthread_barrier_init(&common.barrier1, NULL, common.nthreads);

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
	thread_info	*this = (thread_info *)arg;
	double	start = gettime();
	int	i = 0;
	F	*a = common.a;
	int	n = common.n;
	do {
		if (pthread_self() == info[0].thread) {
			F	pivot = a[i + 2 * n * i];
			for (int j = i; j < 2 * n; j++) {
				a[j + 2 * n * i] /= pivot;
			}
		}
		pthread_barrier_wait(&common.barrier1);
		F	*a = common.a;
		int	n = common.n;
		for (int k = this->min; k < this->max; k++) {
			if (k != i) {
				F	b = a[i + 2 * n * k];
//				for (int j = i + 1; j < 2 * n; j++) {
				for (int j = i; j < 2 * n; j++) {
					a[j + 2 * n * k] -= b * a[j + 2 * n * i];
				}
			}
		}
		pthread_barrier_wait(&common.barrier2);
		i++;
	} while (i < n);
	double	end = gettime();
	if (pthread_self() == info[0].thread) {
		printf("%d,%.6f,%d\n", common.n, end - start, common.nthreads);
	}
	return info;
}

/**
 * \brief threaded Gauss algorithm implementation
 */
void	gauss(int nthreads) {
	start_threads(nthreads);
	join_threads();
}

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

int	main(int argc, char *argv[]) {
	int	n = 10;
	int	c;
	int	nthreads = 1;
	while (EOF != (c = getopt(argc, argv, "p:t:")))
		switch (c) {
		case 'p':
			matrix_precision = atoi(optarg);
			break;
		case 't':
			nthreads = atoi(optarg);
			break;
		}

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
