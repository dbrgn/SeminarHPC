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
	// first check that the file exists, but using stat will also give
	// the file size which can later be used to size a buffer
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

	// read the file data into a memory buffer, allocated on the stack
	// from the file size info in the stat structure received previously
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
	if (NULL == a) {
		fprintf(stderr, "%s:%d: cannot allocate memory: %s\n",
			__FILE__, __LINE__, strerror(errno));
		rc = -1;
		goto cleanup;
	}
	b = float_unit_matrix(n);
	if (NULL == b) {
		fprintf(stderr, "%s:%d: cannot allocate memory: %s\n",
			__FILE__, __LINE__, strerror(errno));
		rc = -1;
		goto cleanup;
	}

	// display the data for small matrices
	if (n <= 10) {
		display_float_matrix(stdout, a, n, n);
	}

	// start time measurement
	double	start = gettime();

	// allocate the OpenCL memory buffers
	cl_mem	input = NULL, output = NULL;

	// create input buffer
	input = clCreateBuffer(context, CL_MEM_READ_ONLY,
		sizeof(float) * n * n, NULL, NULL);
	if (!input) {
		fprintf(stderr, "%s:%d: cannot allocate input buffer\n",
			__FILE__, __LINE__);
		rc = -1;
		goto cleanup;
	}

	// create output buffer
	output = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		sizeof(float) * n * n, NULL, NULL);
	if (!output) {
		fprintf(stderr, "%s:%d: cannot allocate output buffer\n",
			__FILE__, __LINE__);
		rc = -1;
		goto cleanup;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: buffers allocated\n",
			__FILE__, __LINE__);
	}

	// copy the input data to the queue
	int	err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0,
			sizeof(float) * n * n, a, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: cannot enqueue the matrix: %d\n",
			__FILE__, __LINE__, err);
		rc = -1;
		goto cleanup;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: argument data copied\n",
			__FILE__, __LINE__);
	}

	// set the kernel arguments
	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
	err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &n);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: cannot set kernel arguments: %d\n",
			__FILE__, __LINE__, err);
		rc = -1;
		goto cleanup;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: kernel arguments assigned\n",
			__FILE__, __LINE__);
	}

	// compute a suitable work group size
	size_t	global = n;
	if (n <= local) {
		// the matrix size is small als the work group size, so we can 
		// make each row of the matrix into a work group item
		global = n;
	} else {
		// the matrix size is too large. To evenly distribute the
		// work, we have to divide the number of rows into chunks of
		// the same size, so that there are no more chunks than the
		// workgroup size allows. Thus we check all numbers starting
		// at the work group size down to 1 whether it divides the
		// matrix size. This then is a suitable number of chunks
		global = local + 1;
		do {
			global--;
		} while (n % global);
	}
	local = global;
	if (debug) {
		fprintf(stderr, "%s:%d: global: %ld, local: %ld\n",
			__FILE__, __LINE__, global, local);
	}

	// enqueue the kernel
	err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local,
		0, NULL, NULL);
	if (err) {
		fprintf(stderr, "%s:%d: cannot enqueue the kernel: %d\n",
			__FILE__, __LINE__, err);
		rc = -1;
		goto cleanup;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: kernel enqueued\n", __FILE__, __LINE__);
	}

	// read the result data from the queue. This method waits until the
	// the kernel has finished
	err = clEnqueueReadBuffer(commands, output, CL_TRUE, 0,
		sizeof(float) * n * n, b, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: cannot read the data: %d\n",
			__FILE__, __LINE__, err);
		rc = -1;
		goto cleanup;
	}

	// measure end time and compute the elapsed time
	double	end = gettime();
	printf("%d,%f,%d\n", n, end - start, vectorlength);
	fflush(stdout);
	
	// display results
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
	int	gpu = 0;	// whether to use the GPU or the CPU
	int	c;
	int	platform = 0;	// platform number
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
	if (debug) {
		fprintf(stderr, "%s:%d: found %d platforms\n",
			__FILE__, __LINE__, num_platforms);
	}

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
			fprintf(stderr, "platform[%d]: %s/%s %s\n", i, name,
				vname, version);

			// platform extensions
			err = clGetPlatformInfo(id, CL_PLATFORM_EXTENSIONS, 0,
				NULL, &size);
			char *extensions = (char *)alloca(sizeof(char) * size);
			err = clGetPlatformInfo(id, CL_PLATFORM_EXTENSIONS,
				size, extensions, NULL);
			fprintf(stderr, "extensions[%d]: %s\n", i, extensions);
		}
	}

	// if we are on the nvidia platform, then we don't allow the -D
	// flag to the compiler, because the Nvidia platform does not support
	// the printf extension that most other OpenCL implementations support.
	if (PLATFORM_NVIDIA == codes[platform]) {
		Debug = 0;
		gpu = 1;
	}

	// get a compute device of the requested type (GPU/CPU)
	cl_device_id	device_id;
	err = clGetDeviceIDs(platformIds[platform],
		gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id,
		NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: no device id found: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: got device id %p\n",
			__FILE__, __LINE__, device_id);
	}

	// we want to find out some interesting device parameters, in particular
	// we want to know how large memory, is, cache characteristics and
	// simd width (preferred vector size)
	cl_ulong	globalmemsize = 0, localmemsize = 0, cachesize = 0;
	size_t	paramsize = sizeof(globalmemsize);
	size_t	paramsizeret = 0;
	clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_SIZE, 
		paramsize, &globalmemsize, &paramsizeret);
	clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_SIZE, 
		paramsize, &localmemsize, &paramsizeret);
	clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, 
		paramsize, &cachesize, &paramsizeret);

	cl_uint	computeunits = 0;
	paramsize = sizeof(computeunits);
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS,
		paramsize, &computeunits, &paramsizeret);

	cl_uint cacheline = 0;
	paramsize = sizeof(cacheline);
	clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,
		paramsize, &cacheline, &paramsizeret);

	cl_uint	vectorwidth = 0;
	paramsize = sizeof(vectorwidth);
	clGetDeviceInfo(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,
		paramsize, &vectorwidth, &paramsizeret);

	cl_bool	unified = 0;
	paramsize = sizeof(unified);
	clGetDeviceInfo(device_id, CL_DEVICE_HOST_UNIFIED_MEMORY,
		paramsize, &unified, &paramsizeret);

	if (debug) {
		fprintf(stderr, "unified memory:  %s\n",
			(unified) ? "yes" : "no");
		fprintf(stderr, "global memory:   %lu\n", globalmemsize);
		fprintf(stderr, "local memory:    %lu\n", localmemsize);
		fprintf(stderr, "cache size:      %lu\n", cachesize);
		fprintf(stderr, "cache line size: %u\n", cacheline);
		fprintf(stderr, "compute units:   %u\n", computeunits);
		fprintf(stderr, "vector width:    %u\n", vectorwidth);
	}

	// create a context. A number of devices can be attached to a context,
	// but in our case, there is only one
	cl_context	context;
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	if (!context) {
		fprintf(stderr, "%s:%d: cannot create context: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: got context\n", __FILE__, __LINE__);
	}

	// create a command queue. Each device needs its own command queue
	cl_command_queue	commands;
	commands = clCreateCommandQueue(context, device_id, 0, &err);
	if (!commands) {
		fprintf(stderr, "%s:%d: cannot create command queue: %d\n",
			__FILE__, __LINE__,  err);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: got command queue\n",
			__FILE__, __LINE__);
	}

	// create a program from a source file. There is no OpenCL function
	// to read a program from a file, so we supply a uitility function
	// for this purpose.
	cl_program	program = cluCreateProgramWithFile(context,
				"gauss.cl", &err);
	if (!program) {
		fprintf(stderr, "%s:%d: cannot create program: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: got program\n", __FILE__, __LINE__);
	}

	// compile a program. The compiler accepts flags, and we would like
	// to use them to control some aspects of our implementation, in
	// particular the use of vector primitives.
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
		fprintf(stderr, "%s:%d: cannot compile program: %d\n",
			__FILE__, __LINE__, err);
		size_t	l = 32 * 1024;
		char	*log = (char *)malloc(l);
		err = clGetProgramBuildInfo(program, device_id,
			CL_PROGRAM_BUILD_LOG,
			l, log, &l);
		if (err) {
			fprintf(stderr, "%s:%d: cannot retrieve log: %d\n",
				__FILE__, __LINE__, err);
			return EXIT_FAILURE;
		}
		fprintf(stderr, "compile log:\n%*s\n", (int)l, log);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: compiled the program\n",
			__FILE__, __LINE__);
	}

	// create a kernel. This step is necessary because an OpenCL program
	// can contain many kernels. This function then provides a handle
	// to call a particular kernel by its name.
	cl_kernel	kernel = clCreateKernel(program, "invert", &err);
	if (!kernel) {
		fprintf(stderr, "%s:%d: cannot create a kernel: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: kernel created\n", __FILE__, __LINE__);
	}

	// get the local memory requirements of this kernel
	cl_ulong	localmemreq = 0;
	err = clGetKernelWorkGroupInfo(kernel, device_id,
		CL_KERNEL_LOCAL_MEM_SIZE, sizeof(localmemreq), &localmemreq,
		NULL);
	if (debug) {
		fprintf(stderr, "local mem req:   %lu\n", localmemreq);
	}

	// get workgroup info. When 
	size_t	local;
	err = clGetKernelWorkGroupInfo(kernel, device_id,
		CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: can't get kernel work group info: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "work group size: %lu\n", local);
	}

	// for each subsequent argument, perform a Gauss experiment
	while (optind < argc) {
		size_t	n = atoi(argv[optind]);
		if (n <= 1) {
			fprintf(stderr, "%s:%d: %s not a valid problem size, "
				"skipping\n", __FILE__, __LINE__, argv[optind]);
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
