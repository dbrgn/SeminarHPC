/*
 * gauss.c -- opencl implementation of Gauss algorithm
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <OpenCL/opencl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <common.h>

int	debug = 0;

/*
 * \brief Auxiliary function to read OpenCL code from a file
 *
 * This function reads the contents of a file and invokes the
 * clCreateProgramWithSource to create a program.
 */
cl_program	cluCreateProgramWithFile(cl_context context,
			const char *filename, cl_int *err) {
	// first check that the file exists
	struct stat	sb;
	if (stat(filename, &sb) < 0) {
		*err = -1;
		return NULL;
	}

	// open the file for reading
	int	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		*err = -1;
		return NULL;
	}

	// read the file data into a memory buffer
	size_t	length[1];
	length[0] = sb.st_size;
	char	*source[1];
	source[0] = alloca(sb.st_size);
	if (sb.st_size != read(fd, source[0], sb.st_size)) {
		*err = -1;
		return NULL;
	}
	close(fd);

	// use the clCreateProgramWithSource function to create the program
	cl_program	program;
	program = clCreateProgramWithSource(context, 1, source, length, err);

	return program;
}

/**
 * \brief Main function
 */
int	main(int argc, char *argv[]) {
	// parse command line arguments
	int	gpu = 0;
	int	c;
	unsigned int	n = 10;
	while (EOF != (c = getopt(argc, argv, "gdn:p:")))
		switch (c) {
		case 'd':
			debug = 1;
			break;
		case 'g':
			gpu = 1;
			break;
		case 'n':
			n = atoi(optarg);
			if (n == 0) {
				fprintf(stderr, "not a suitable size: %s\n",
					optarg);
				return EXIT_FAILURE;
			}
			break;
		case 'p':
			matrix_precision = atoi(optarg);
			break;
		}

	// initialize the data
	float	*a, *b;
	a = random_float_matrix(n, n);
	b = float_unit_matrix(n);
	if ((NULL == a) || (NULL == b)) {
		fprintf(stderr, "cannot allocate memory: %s\n",
			strerror(errno));
		return EXIT_FAILURE;
	}

	if (n <= 10) {
		display_float_matrix(stdout, a, n, n);
	}

	// start OpenCL initialization
	int	err;

	// get a compute device
	cl_device_id	device_id;
	err = clGetDeviceIDs(NULL,
		gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id,
		NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "no device id found: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { printf("got device id %p\n", device_id); }

	// create a context
	cl_context	context;
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	if (!context) {
		fprintf(stderr, "cannot create context: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { printf("got context\n"); }

	// create a command queue
	cl_command_queue	commands;
	commands = clCreateCommandQueue(context, device_id, 0, &err);
	if (!commands) {
		fprintf(stderr, "cannot create command queue: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { printf("got command queue\n"); }

	// create a program from a source file
	cl_program	program = cluCreateProgramWithFile(context,
				"gauss.cl", &err);
	if (!program) {
		fprintf(stderr, "cannot create program: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { printf("got program\n"); }

	// compile a program
	err = clBuildProgram(program, 1, &device_id,
		(debug) ? "-DDEBUG" : NULL, NULL, NULL);
	if (err) {
		fprintf(stderr, "cannot compile program: %d\n", err);
		size_t	l = 32 * 1024;
		char	*log = (char *)malloc(l);
		err = clGetProgramBuildInfo(program, device_id,
			CL_PROGRAM_BUILD_LOG,
			l, log, &l);
		if (err) {
			fprintf(stderr, "cannot retrieve log: %d\n", err);
			return EXIT_FAILURE;
		}
		fprintf(stderr, "compile log:\n%*s\n", l, log);
		return EXIT_FAILURE;
	}
	if (debug) { printf("compiled the program\n"); }

	// create a kernel
	cl_kernel	kernel = clCreateKernel(program, "invert", &err);
	if (!kernel) {
		fprintf(stderr, "cannot create a kernel: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { printf("kernel created\n"); }

	// create input and output buffers
	cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY,
		sizeof(float) * n * n, NULL, NULL);
	cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		sizeof(float) * n * n, NULL, NULL);
	if ((!input) || (!output)) {
		fprintf(stderr, "cannot allocate buffers\n");
		return EXIT_FAILURE;
	}
	if (debug) { printf("buffers allocated\n"); }

	// copy the info 
	err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0,
		sizeof(float) * n * n, a, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "cannot enqueue the matrix: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { printf("argument data copied\n"); }

	// kernel arguments
	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
	err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &n);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "cannot set kernel arguments: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { printf("kernel arguments assigned\n"); }

	// get workgroup info
	size_t	local;
	err = clGetKernelWorkGroupInfo(kernel, device_id,
		CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	if (err != CL_SUCCESS) {
		printf("cannot get kernel work group info: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { printf("got work group size: %lu\n", local); }

	// compute a suitable work group size
	size_t	global = n;
	if (global < local) {
		global = local;
		local = 1;
	} else {
		global = 1;
		local = 1;
	}

	// enqueue the kernel
	err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local,
		0, NULL, NULL);
	if (err) {
		fprintf(stderr, "cannot enqueue the kernel: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { printf("kernel enqueued\n"); }

	// read the result data
	err = clEnqueueReadBuffer(commands, output, CL_TRUE, 0,
		sizeof(float) * n * n, b, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "cannot read the data: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { printf("result read\n"); }

	if (n <= 10) {
		display_float_matrix(stdout, b, n, n);
	}

	// release all the objects
	clReleaseMemObject(input);
	clReleaseMemObject(output);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(commands);
	clReleaseContext(context);

	return EXIT_SUCCESS;
}
