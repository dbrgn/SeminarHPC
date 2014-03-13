/*
 * gauss.cl -- opencl kernel implementing gauss algorithm
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
 *			[6]: number of iterations
 *                      [7]: number of attracting points
 *                      [8]: attracting point[0] real part
 *                      [9]: attricting point[1] imaginary part
 *                      additional attricting points follow 
 * \param output	output array width x height
 *                      width = get_global_size(0)
 *                      height = get_global_size(1)
 */
__kernel void	iterate(__global double *parameters, __global int *output) {
	// compute the initial value for the iteration
	__private double	originx = parameters[0];
	__private double	originy = parameters[1];
	__private double	hx = parameters[2];
	__private double	hy = parameters[3];
	__private double2	c;
	c.x = parameters[4];
	c.y = parameters[5];
	__private int	iterations = parameters[6];

	__private double2	z;
	z.x = get_global_id(0) * hx;
	z.y = get_global_id(1) * hy;

	// now perform forward iteration for <iterations> steps
	__private int	i;
	for (i = 0; i < iterations; i++) {
		__private double	x = z.x * z.x - z.y * z.y + c.x;
		z.y = 2 * z.x * z.y + c.y;
		z.x = x;
	}

	// find out the nearest point from the list
	__private int	npoints = parameters[7];
	__private int	closest_index = 0;
	__private double	dmin = 100000;
	for (i = 0; i < npoints; i++) {
		__private double2	p;
		p.x = parameters[8 + 2 * i];
		p.y = parameters[8 + 2 * i + 1];
		__private double	d = distance(p, z);
		if (d < dmin) {
			closest_index = i;
			dmin = d;
		}
	}

	// we have now found the index of the closest point, so lets return it
	output[get_global_id(0) + get_global_id(1) * get_global_size(0)]
		= closest_index;
}

