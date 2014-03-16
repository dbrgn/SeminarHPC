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

/*
 * Perform Julia inverse function computation
 * \param c	parameter f_c(z)=z^2+c
 * \param z	argument 
 * \param side	selects one of complex roots
 */
double2	juliaroot1(double2 c, double2 z, bool side) {
	double2	zz = z - c;
	double	l = sqrt(length(zz));
	double	phi = (atan2(zz.y, zz.x) / 2.) + ((side) ? M_PI : 0);
	double	cc;
	zz.y = sincos(phi, &cc);
	zz.x = cc;
	zz = l * zz;
	return zz;
}

/*
 * Alterantive implementation of the root function using Newton's algorithm
 * to compute the roots. At least on AMD, this is actually slower, which may
 * be due to the fast atan2 and sin/cos available on the CPU. It could
 * be entirely different on a GPU
 */
double2	juliaroot2(double2 c, double2 z0, bool side) {
	double2	z = 1;
	double2	a = c - z0;
	double	l = dot(z, z);
	int	i;
	for (i = 0; i < 20; i++) {
		double2	v;
		v.x = a.x * z.x + a.y * z.y;
		v.y = -a.x * z.y + a.y * z.x;
		z = (z - v / l) / 2;
//printf("[%d,%d] %10.6f + %10.6fi\n", get_global_id(0), i, z.x, z.y);
	}
	return (side) ? -z : z;
}

#define juliaroot	juliaroot1

/*
 * register a point in the output image
 */
void	registerpoint(double2 z, double2 origin, double2 h, int2 size,
	__global unsigned char *output) {
//printf("register %f + %fi\n", z.x, z.y);
	double2	p = floor((z - origin) / h);
	int2	point;
	point.x = p.x;
	point.y = p.y;
	int2	zero = 0;
	if (all(zero <= point) && all(point < size)) {
		int	idx = point.x + size.x * point.y;
		unsigned char	v = output[idx];
		if (v < 255) {
			output[idx] = v + 1;
		}
	}
}

/*
 * Park-Miller random number generator
 */
__constant double a = 16807;
__constant double m = 2147483647;
__constant double reciprocal_m = 1.0 / 2147473647;

int	random(int seed) {
	__private double	temp = seed * a;
	return (int)(temp - m * floor(temp * reciprocal_m));
}

/*
 * Advance backward iteration for a all bits of a random number
 */
double2	advance(double2 z, double2 c, int seed) {
	int	j = 31;
	double2	zz = z;
	while (j--) {
		zz = juliaroot(c, zz, (0x1 & seed));
		seed >>= 1;
	}
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
 *			[10]: initial point real part
 *			[11]: initial point imaginary part
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
	__private double2	z;
	z.x = parameters[10];
	z.y = parameters[11];

	// park-miller implementation
	__private int	seed = get_global_id(0);
	__private unsigned short	bits = seed % 0x10000;
	__private double	temp;

	// perform a few random iterations
	__private int	i;
	for (i = 0; i < 10; i++) {
		seed = random(seed);
	}

	// perform a number of backward iterations
	for (i = 0; i < initial_iterations; i++) {
		z = advance(z, c, seed);
		seed = random(seed);
	}

	// perform a number of backward iterations, but register the points
	// you find during these iterations
	for (i = 0; i < iterations; i++) {
		// go through the bits of the work id
		__private unsigned short	b = bits;
		__private unsigned char	j = 16;
		while (j) {
			z = juliaroot(c, z, (0x1 & b));
			registerpoint(z, origin, h, size, output);
			j--;
			b >>= 1;
		}

		// use the last bit of the random number
		z = juliaroot(c, z, (0x1 & seed));
		registerpoint(z, origin, h, size, output);
		
		// get the next random number
		seed = random(seed);
	}
}
