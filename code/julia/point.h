/*
 * point.h  -- point parsing routine
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _point_h
#define _point_h

#include <complex.h>

extern int	parse_point(const char *arg, double *values);
extern int	parse_cpoint(const char *arg, double complex *v);

#endif /* _point_h */
