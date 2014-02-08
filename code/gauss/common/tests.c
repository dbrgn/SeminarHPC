/*
 * tests.c -- simple test program to exercise some of the common functions
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "common.h"

void	timetest() {
	init_gettime();
	double	t1, t2;
	t2 = gettime();
	do {
		t1 = t2;
		usleep(1);
		t2 = gettime();
#if 0
		double	delta = t2 - t1;
		if (fabs(delta - 0.000001) > 0.000002) {
			printf("t1 = %.6f, t2 = %.6f, delta = %.6f\n",
				t1, t2, delta);
		}
#endif
	} while (t2 >= t1);
	printf("t1 = %.6f, t2 = %.6f\n", t1, t2);
}

void	random_matrix_test() {
	float	*a = random_float_matrix(10, 20);
	display_float_matrix(stdout, a, 10, 20);
}

int	main(int argc, char *argv[]) {
	random_matrix_test();
	return EXIT_SUCCESS;
}
