/*
 * julia.cl -- opencl kernel for julia set computation
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

#ifndef LIMIT
#define LIMIT 65535
#endif

/**
 * \brief Kernel for Julia set computation
 *
 * \param parameters	array containing parameters for the computation
 *                      The parameters are:
 *                      [0]: originx
 *                      [1]: originy
 *                      [2]: grid constant in x direction
 *                      [3]: grid constant in y direction
 *                      [4]: Julia parameter real part
 *                      [5]: Julia parameter imaginary part
 *			[6]: escape distance
 *                      additional attricting points follow 
 * \param output	output array width x height
 *                      width = get_global_size(0)
 *                      height = get_global_size(1)
 */
__kernel void	iterate(__global double *parameters,
	__global unsigned short *output) {
	// compute the initial value for the iteration
	__private double	originx = parameters[0];
	__private double	originy = parameters[1];
	__private double	hx = parameters[2];
	__private double	hy = parameters[3];
	__private double2	c;
	c.x = parameters[4];
	c.y = parameters[5];
	__private double	escape_distance = parameters[6];

	__private double2	z;
	z.x = originx + get_global_id(0) * hx;
	z.y = originy + get_global_id(1) * hy;

	// now perform forward iteration for <iterations> steps
	__private int	i;
	double	l = length(z);
	i = 0;
	while ((l < escape_distance) && (i < LIMIT)) {
		__private double	x = z.x * z.x - z.y * z.y + c.x;
		z.y = 2 * z.x * z.y + c.y;
		z.x = x;
		l = length(z);
		i++;
	}

	int	idx = get_global_id(0) + get_global_id(1) * get_global_size(0);
	if (i == LIMIT) {
		output[idx] = 0;
	} else {
		output[idx] = i;
	}
}

