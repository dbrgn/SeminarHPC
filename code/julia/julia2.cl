/*
 * julia2.cl -- opencl kernel for the computation of julia sets
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#pragma OPENCL EXTENSION cl_khr_fp64: enable

#ifdef AMD
#pragma OPENCL EXTENSION cl_amd_printf : enable
#endif

#ifdef INTEL
#pragma OPENCL EXTENSION cl_intel_printf : enable
#endif

double2	juliaroot(double2 c, double2 z, bool side) {
	double2	zz = z - c;
	double	l = sqrt(length(zz));
	double	phi = (atan2(zz.y, zz.x) / 2.) + ((side) ? M_PI : 0);
	double	cc;
	zz.y = sincos(phi, &cc);
	zz.x = cc;
	zz = l * zz;
	return zz;
}

/**
 * \brief Kernel for Julia set computation
 *
 * \param parameters	array containing parameters for the computation
 *                      The parameters are:
 *                      [0]: width
 *                      [1]: height
 *                      [2]: originx
 *                      [3]: originy
 *                      [4]: grid constant in x direction
 *                      [5]: grid constant in y direction
 *                      [6]: Julia parameter real part
 *                      [7]: Julia parameter imaginary part
 *			[8]: number of initial iterations
 *			[9]: number of iterations
 *                      additional attricting points follow 
 * \param output	output array width x height
 *                      width = get_global_size(0)
 *                      height = get_global_size(1)
 */
__kernel void	iterate(__global double *parameters,
	__global unsigned char *output) {
	// compute the initial value for the iteration
	int2	size;
	size.x = parameters[0];
	size.y = parameters[1];
	__private double2	origin;
	origin.x = parameters[2];
	origin.y = parameters[3];
	__private double2	h;
	h.x = parameters[4];
	h.y = parameters[5];
	__private double2	c;
	c.x = parameters[6];
	c.y = parameters[7];
	__private int	initial_iterations = parameters[8];
	__private int	iterations = parameters[9];

	// backward iteration variable
	__private double2	z = 0;

	// park-miller implementation
	double const	a = 16807;
	double const	m = 2147483647;
	double const	reciprocal_m = 1.0 / m;
	__private int	seed = get_global_id(0);
	__private double	temp;

	// perform a few random iterations
	__private int	i;
	for (i = 0; i < 10; i++) {
		temp = seed * a;
		seed = (int)(temp - m * floor(temp * reciprocal_m));
	}

	// perform a number of backward iterations
	for (i = 0; i < iterations; i++) {
		z = juliaroot(c, z, (0x1 & seed));
		temp = seed * a;
		seed = (int)(temp - m * floor(temp * reciprocal_m));
	}

	// perform a number of backward iterations, but register the points
	// you find during these iterations
	int2	zero = 0;
	for (i = 0; i < iterations; i++) {
		z = juliaroot(c, z, (0x1 & seed));
		double2	p = floor((z - origin) / h);
		int2	point;
		point.x = p.x;
		point.y = p.y;
		if (all(zero <= point) && all(point < size)) {
			int	idx = point.x + size.x * point.y;
			unsigned char	v = output[idx];
			if (v < 255) {
				output[idx] = v + 1;
			}
		}
		temp = seed * a;
		seed = (int)(temp - m * floor(temp * reciprocal_m));
	}
}
