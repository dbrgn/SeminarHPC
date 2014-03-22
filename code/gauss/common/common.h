/*
 * common.h -- common functions 
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _common_h
#define _common_h

#include <stdio.h>

#define	M(_a, _n, _i, _j)	_a[(_j) + (_n) * (_i)]

#ifdef __cplusplus
extern "C" {
#endif

extern float	*random_float_matrix(int n, int m);
extern double	*random_double_matrix(int n, int m);

extern float	*float_unit_matrix(int n);
extern double	*double_unit_matrix(int n);

extern int	matrix_precision;
extern char	*matrix_prefix;

extern void	display_float_matrix(FILE *f, float *a, int n, int m);
extern void	display_double_matrix(FILE *f, double *a, int n, int m);

extern void	init_gettime();
extern double	gettime();

#ifdef __cplusplus
}
#endif

#endif /* _common_h */
