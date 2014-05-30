/*
 * boundary.h -- functions related to boundary data interchange
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _boudnary_h
#define _boundary_h

#include "domain.h"

extern void	exchange_boundaries(udata_t *u, int tag);

#endif /* _boundary_h */
