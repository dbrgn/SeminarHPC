/*
 * domain.c -- domain related stuff for the MPI implementation
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _domain_h
#define _domain_h

#include <mpi.h>

typedef struct {
	double	*u;	// u values
	double	*b;	// b values
	double	*left;	// left border (left neighbor)
	double	*right;	// right border (right neighbor)
	double	*top;	// top border (top neighbor)
	double	*bottom;// bottom border (bottom neighbor)
	double	*send_left;
	double	*send_right;
	double	*send_top;
	double	*send_bottom;
	MPI_Request	left_request;
	MPI_Request	right_request;
	MPI_Request	top_request;
	MPI_Request	bottom_request;
	int	width;	// width of this part of u
	int	height;	// height of this part of u
	int	length;	// number of values in this patch
	int	rh;	// range index in x direction
	int	rv;	// range index in y direction
	int	*ranges;
	int	nx;	// number of ranges in x direction
	int	ny;	// number of ranges in y direction
	int	rank;	// rank of this processes
	int dimension; // dimension of this processes
	int algorithm; // select algorithm; Jacobi (0), GaussSeidel (1)
	int maxsteps;	// steps for algorithm
	int picturesteps;	// takes a picture every x step
	double	ht;	// time step
	double	h2;	// 2h^2_x, used in laplacian computation
	double	h;  // set h = 1 / n, used in laplacian computation
} udata_t;

extern double	*doublevector(int size);
extern void	allocate_u(udata_t *u);
extern void	free_u(udata_t *u);
extern double	U(const udata_t *u, int i, int j);

#endif /* _domain_h */
