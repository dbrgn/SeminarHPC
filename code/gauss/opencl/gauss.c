/*
 * gauss.c -- opencl implementation of Gauss algorithm
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <CL/opencl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <common.h>
#include <fcntl.h>
#include <unistd.h>
#include <alloca.h>
#include <getopt.h>
#include <common.h>

int	debug = 0;
int	vectorlength = 1;

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
	program = clCreateProgramWithSource(context, 1, (const char **)source,
		length, err);

	return program;
}

/**
 * \brief Perform a gauss experiment with a matrix of a given size
 */
int	gauss_experiment(cl_context context, cl_command_queue commands,
		cl_kernel kernel, size_t local, unsigned int n) {
	int	rc = 0;

	// initialize the data, with the right size
	float	*a = NULL, *b = NULL;
	a = random_float_matrix(n, n);
	b = float_unit_matrix(n);
	if ((NULL == a) || (NULL == b)) {
		fprintf(stderr, "cannot allocate memory: %s\n",
			strerror(errno));
		return EXIT_FAILURE;
	}

	// display the data
	if (n <= 10) {
		display_float_matrix(stdout, a, n, n);
	}

	// start time measurement
	double	start = gettime();

	// allocate buffers
	cl_mem	input = NULL, output = NULL;

	// create input buffer
	input = clCreateBuffer(context, CL_MEM_READ_ONLY,
		sizeof(float) * n * n, NULL, NULL);
	if (!input) {
		fprintf(stderr, "cannot allocate input buffer\n");
		rc = -1;
		goto cleanup;
	}

	// create output buffer
	output = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		sizeof(float) * n * n, NULL, NULL);
	if (!output) {
		fprintf(stderr, "cannot allocate output buffer\n");
		rc = -1;
		goto cleanup;
	}
	if (debug) { fprintf(stderr, "buffers allocated\n"); }

	// copy the info 
	int	err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0,
			sizeof(float) * n * n, a, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "cannot enqueue the matrix: %d\n", err);
		rc = -1;
		goto cleanup;
	}
	if (debug) { fprintf(stderr, "argument data copied\n"); }

	// kernel arguments
	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
	err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &n);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "cannot set kernel arguments: %d\n", err);
		rc = -1;
		goto cleanup;
	}
	if (debug) { fprintf(stderr, "kernel arguments assigned\n"); }

	// compute a suitable work group size
	size_t	global = n;
	if (n <= local) {
		local = n;
		global = n;
	} else {
		// find the largest divisor of n that is smaller than local
		global = local + 1;
		do {
			global--;
		} while (n % global);
		local = global;
	}
	if (debug) {
		fprintf(stderr, "global: %ld, local: %ld\n", global, local);
	}

	// enqueue the kernel
	err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local,
		0, NULL, NULL);
	if (err) {
		fprintf(stderr, "cannot enqueue the kernel: %d\n", err);
		rc = -1;
		goto cleanup;
	}
	if (debug) { fprintf(stderr, "kernel enqueued\n"); }

	// read the result data
	err = clEnqueueReadBuffer(commands, output, CL_TRUE, 0,
		sizeof(float) * n * n, b, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "cannot read the data: %d\n", err);
		rc = -1;
		goto cleanup;
	}

	// end time
	double	end = gettime();
	printf("%d,%f,%d\n", n, end - start,vectorlength);
	fflush(stdout);
	
	// display results
	if (debug) { fprintf(stderr, "result read\n"); }

	if (n <= 10) {
		display_float_matrix(stdout, b, n, n);
	}

cleanup:
	if (input) {
		clReleaseMemObject(input);
	}
	if (output) {
		clReleaseMemObject(output);
	}
	if (b) {
		free(b);
	}
	if (a) {
		free(a);
	}


	// experiment was successful
	return rc;
}

/**
 * \brief Main function
 */
int	main(int argc, char *argv[]) {
	// parse command line arguments
	int	gpu = 0;
	int	c;
	int	platform = 0;
	int	Debug = 0;
	while (EOF != (c = getopt(argc, argv, "gdp:P:Dv:")))
		switch (c) {
		case 'd':
			debug = 1;
			break;
		case 'D':
			Debug = 1;
			break;
		case 'g':
			gpu = 1;
			break;
		case 'p':
			matrix_precision = atoi(optarg);
			break;
		case 'P':
			platform = atoi(optarg);
			break;
		case 'v':
			vectorlength = atoi(optarg);
			break;
		}

	// initialize timer
	init_gettime();

	// start OpenCL initialization
	int	err;

	// first get the platform info
	cl_uint	num_platforms;
	err = clGetPlatformIDs(0, NULL, &num_platforms);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "cannot get the number of platforms\n");
		return EXIT_FAILURE;
	}
	if (debug) { fprintf(stderr, "found %d platforms\n", num_platforms); }

	cl_platform_id	*platformIds = (cl_platform_id *)malloc(
		num_platforms * sizeof(cl_platform_id));
	err = clGetPlatformIDs(num_platforms, platformIds, 0);

	// display information about the platforms
	typedef enum {
		PLATFORM_AMD, PLATFORM_NVIDIA, PLATFORM_INTEL
	} platformcode;
	platformcode	*codes = (platformcode *)malloc(sizeof(platformcode)
					* num_platforms);
	if (debug) {
		for (int i = 0; i < num_platforms; i++) {
			size_t size;

			// platform name
			cl_platform_id	id = platformIds[i];
			err = clGetPlatformInfo(id, CL_PLATFORM_NAME, 0, NULL,
				&size);
			char * name = (char *)alloca(sizeof(char) * size);
			err = clGetPlatformInfo(id, CL_PLATFORM_NAME, size,
				name, NULL);

			if (0 == strncmp(name, "NVIDIA", 6)) {
				codes[i] = PLATFORM_NVIDIA;
			}
			if (0 == strncmp(name, "AMD", 3)) {
				codes[i] = PLATFORM_AMD;
			}
			if (0 == strncmp(name, "Intel", 5)) {
				codes[i] = PLATFORM_INTEL;
			}

			// platform vendor
			err = clGetPlatformInfo(id, CL_PLATFORM_VENDOR, 0, NULL,
				&size);
			char * vname = (char *)alloca(sizeof(char) * size);
			err = clGetPlatformInfo(id, CL_PLATFORM_VENDOR, size,
				vname, NULL);

			// platform version
			err = clGetPlatformInfo(id, CL_PLATFORM_VERSION, 0,
				NULL, &size);
			char *version = (char *)alloca(sizeof(char) * size);
			err = clGetPlatformInfo(id, CL_PLATFORM_VERSION, size,
				version, NULL);
			fprintf(stderr, "platform %d: %s/%s %s\n", i, name,
				vname, version);

			// platform extensions
			err = clGetPlatformInfo(id, CL_PLATFORM_EXTENSIONS, 0,
				NULL, &size);
			char *extensions = (char *)alloca(sizeof(char) * size);
			err = clGetPlatformInfo(id, CL_PLATFORM_EXTENSIONS,
				size, extensions, NULL);
			fprintf(stderr, "extensions %d: %s\n", i, extensions);
		}
	}

	// if we are on the nvidia platform, then we don't allow the -D
	// flat to the compiler
	if (PLATFORM_NVIDIA == codes[platform]) {
		Debug = 0;
		gpu = 1;
	}

	// get a compute device
	cl_device_id	device_id;
	err = clGetDeviceIDs(platformIds[platform],
		gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id,
		NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "no device id found: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { fprintf(stderr, "got device id %p\n", device_id); }

	// create a context
	cl_context	context;
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	if (!context) {
		fprintf(stderr, "cannot create context: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { fprintf(stderr, "got context\n"); }

	// create a command queue
	cl_command_queue	commands;
	commands = clCreateCommandQueue(context, device_id, 0, &err);
	if (!commands) {
		fprintf(stderr, "cannot create command queue: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { fprintf(stderr, "got command queue\n"); }

	// create a program from a source file
	cl_program	program = cluCreateProgramWithFile(context,
				"gauss.cl", &err);
	if (!program) {
		fprintf(stderr, "cannot create program: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { fprintf(stderr, "got program\n"); }

	// compile a program
	char	flags[200];
	memset(flags, 0, sizeof(flags));
	switch (codes[platform]) {
	case PLATFORM_AMD:
		strcpy(flags, "-DAMD");
		break;
	case PLATFORM_INTEL:
		strcpy(flags, "-DINTEL");
		break;
	case PLATFORM_NVIDIA:
		strcpy(flags, "-DNVIDIA");
		break;
	}
	if (Debug) {
		strcat(flags, " -DDEBUG");
	}
	switch (vectorlength) {
	case 2:
		strcat(flags, " -DVECTOR2");
		break;
	case 4:
		strcat(flags, " -DVECTOR4");
		break;
	case 8:
		strcat(flags, " -DVECTOR8");
		break;
	case 16:
		strcat(flags, " -DVECTOR16");
		break;
	}

	err = clBuildProgram(program, 1, &device_id, flags, NULL, NULL);
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
		fprintf(stderr, "compile log:\n%*s\n", (int)l, log);
		return EXIT_FAILURE;
	}
	if (debug) { fprintf(stderr, "compiled the program\n"); }

	// create a kernel
	cl_kernel	kernel = clCreateKernel(program, "invert", &err);
	if (!kernel) {
		fprintf(stderr, "cannot create a kernel: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { fprintf(stderr, "kernel created\n"); }

	// get workgroup info
	size_t	local;
	err = clGetKernelWorkGroupInfo(kernel, device_id,
		CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "cannot get kernel work group info: %d\n", err);
		return EXIT_FAILURE;
	}
	if (debug) { fprintf(stderr, "got work group size: %lu\n", local); }

	// for each subsequent argument, perform a Gauss experiment
	while (optind < argc) {
		size_t	n = atoi(argv[optind]);
		if (n <= 1) {
			fprintf(stderr, "%s not a valid problem size, "
				"skipping\n", argv[optind]);
		} else {
			gauss_experiment(context, commands, kernel, local, n);
			optind++;
		}
	}

	// release all the objects
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(commands);
	clReleaseContext(context);

	return EXIT_SUCCESS;
}
