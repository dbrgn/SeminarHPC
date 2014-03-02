/*
 * iteration.h -- iteration and computation related functions
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _iteration_h
#define _iteration_h

#include "domain.h"

extern void	compute_b(udata_t *u);
extern void	iterate_u(double *unew, const udata_t *u);

#endif /* _iteration_h */
