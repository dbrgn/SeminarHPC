/*
 * gauss.cl -- opencl kernel implementing gauss algorithm
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifdef AMD
#pragma OPENCL EXTENSION cl_amd_printf : enable
#endif

#ifdef INTEL
#pragma OPENCL EXTENSION cl_intel_printf : enable
#endif

#ifdef DEBUG
#if defined(AMD) or defined(INTEL)
#define DEBUGGING
// display function used for debugging
void	display(__global float *a, const unsigned int n);
void	display(__global float *a, const unsigned int n) {
	unsigned int	i, j;
	printf("[\n");
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (j) { printf(","); }
			printf("%6.3f", a[j + n * i]);
		}
		printf("\n");
	}
	printf("];\n");
}
#else
#endif
#endif

/**
 * \brief Kernel for Gauss algorithm
 *
 * \param input		input array n x n
 * \param output	output array n x n
 * \param n		matrix dimension
 */
__kernel void	invert(__global float *input, __global float *output,
	const unsigned int n) {
	// compute the range of indices this work item is reponsible for
	__local size_t	local_size;
	__private unsigned int	min_row;
	__private unsigned int	max_row;

	// compute the dimensions of the block that this thread is
	// responsible for computing
	local_size = get_local_size(0);
	unsigned int	blocksize = n / local_size;
	min_row = get_local_id(0) * blocksize;
	max_row = min_row + blocksize;

	// initialize the output array with a unit matrix
	unsigned int	i, j;
	for (i = min_row; i < max_row; i++) {
		for (j = 0; j < n; j++) {
			output[j + i * n] = (i == j) ? 1 : 0;
		}
	}

	// wait for all threads to complete initialization of their
	// part of the block
	barrier(CLK_GLOBAL_MEM_FENCE);

#if DEBUGGING
	if (0 == get_local_id(0)) {
		printf("initialized output matrix:\n");
		display(output, n);
	}
	barrier(CLK_GLOBAL_MEM_FENCE);
#endif

	i = 0;
	while (i < n) {
		// local id 0 does the pivot operation
		if (get_local_id(0) == 0) {
			float	pivot = 1 / input[i + i * n];
			for (j = 0; j < n; j++) {
				input[j + i * n] = input[j + i * n] * pivot;
				output[j + i * n] = output[j + i * n] * pivot;
			}
		}


#ifdef DEBUGGING
		barrier(CLK_GLOBAL_MEM_FENCE);

		if (0 == get_local_id(0)) {
			printf("after pivot operation on row %d\n", i);
			display(input, n);
			display(output, n);
		}
#endif

		// barrier to wait for the pivot row operation to complete
		barrier(CLK_GLOBAL_MEM_FENCE);

		// now perform the row operations all over the matrix
		unsigned int	k = min_row;
		for (; k < max_row; k++) {
			__global float	*outp = output + n * i;
			__global float	*inp = input + n * i;
			if (k != i) {
				j = 0; 
				__global float	*out = output + n * k;
				__global float	*in = input + n * k;
				float	b = in[i];
#if defined(VECTOR2)
				// do as many operations as possible using
				// vector operations, as they allow for more
				// parallelism
				unsigned int l = 0;
				while (j + 2 < n) {
					float2	v = vload2(l, out);
					float2	p = vload2(l, outp);
					v = v - b * p;
					vstore2(v, l, out);

					v = vload2(l, in);
					p = vload2(l, inp);
					v -= b * p;
					vstore2(v, l, in);

					j += 2;
					l++;
				}
#endif
#if defined(VECTOR4)
				// do as many operations as possible using
				// vector operations, as they allow for more
				// parallelism
				unsigned int l = 0;
				while (j + 4 < n) {
					float4	v = vload4(l, out);
					float4	p = vload4(l, outp);
					v = v - b * p;
					vstore4(v, l, out);

					v = vload4(l, in);
					p = vload4(l, inp);
					v -= b * p;
					vstore4(v, l, in);

					j += 4;
					l++;
				}
#endif
#if defined(VECTOR8)
				// do as many operations as possible using
				// vector operations, as they allow for more
				// parallelism
				unsigned int l = 0;
				while (j + 8 < n) {
					float8	v = vload8(l, out);
					float8	p = vload8(l, outp);
					v = v - b * p;
					vstore8(v, l, out);

					v = vload8(l, in);
					p = vload8(l, inp);
					v -= b * p;
					vstore8(v, l, in);

					j += 8;
					l++;
				}
#endif
#if defined(VECTOR16)
				// do as many operations as possible using
				// vector operations, as they allow for more
				// parallelism
				unsigned int l = 0;
				while (j + 16 < n) {
					float16	v = vload16(l, out);
					float16	p = vload16(l, outp);
					v = v - b * p;
					vstore16(v, l, out);

					v = vload16(l, in);
					p = vload16(l, inp);
					v -= b * p;
					vstore16(v, l, in);

					j += 16;
					l++;
				}
#endif
				// do the remining operations by scalar
				// operations
				while (j < n) {
					out[j] -= b * outp[j];
					in[j] -= b * inp[j];
					j++;
				}
			}
		}

#ifdef DEBUGING
		barrier(CLK_GLOBAL_MEM_FENCE);
		if (0 == get_local_id(0)) {
			printf("after row operations using row %d\n", i);
			display(input, n);
			display(output, n);
		}
#endif

		// barrier to wait for the row operations to complete
		barrier(CLK_GLOBAL_MEM_FENCE);

		// go to the next 
		i++;
	}
}

