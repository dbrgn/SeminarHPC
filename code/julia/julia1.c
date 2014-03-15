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
#include <fcntl.h>
#include <unistd.h>
#include <alloca.h>
#include <getopt.h>
#include <common.h>
#include <fitsio.h>
#include "point.h"
#include "color.h"
#include "mono.h"

int	debug = 0;

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
 * \brief Main function
 */
int	main(int argc, char *argv[]) {
	// parse command line arguments
	int	gpu = 0;	// whether to use the GPU or the CPU
	int	C;
	int	platform = 0;	// platform number
	int	color = 0;
	int	Debug = 0;
	int	height = 2048;
	int	width = 2048;
	int	sx = 32;
	int	sy = 32;
	double	c[2] = { -0.52, 0.57 };
	double	origin[2] = { -2, -2 };
	double	size[2] = { 4, 4 };
	double	boundary = 1000;
	double	gamma = 1.0;
	int	expand = 0;
	while (EOF != (C = getopt(argc, argv, "b:Cc:dg:GP:Dv:w:h:o:S:s:t:e")))
		switch (C) {
		case 'b':
			boundary = atof(optarg);
			break;
		case 'd':
			debug = 1;
			break;
		case 'D':
			Debug = 1;
			break;
		case 'e':
			expand = 1;
			break;
		case 'G':
			gpu = 1;
			break;
		case 'g':
			gamma = atof(optarg);
			break;
		case 'P':
			platform = atoi(optarg);
			break;
		case 'w':
			width = atoi(optarg);
			break;
		case 'h':
			height = atoi(optarg);
			break;
		case 'o':
			if (parse_point(optarg, origin) < 0) {
				fprintf(stderr, "bad origin argument: %s\n", optarg);
				return EXIT_FAILURE;
			}
			break;
		case 'S':
			if (parse_point(optarg, size) < 0) {
				fprintf(stderr, "bad size argument: %s\n", optarg);
				return EXIT_FAILURE;
			}
			break;
		case 's':
			sx = atoi(optarg);
			break;
		case 't':
			sy = atoi(optarg);
			break;
		case 'c':
			if (parse_point(optarg, c) < 0) {
				fprintf(stderr, "argument format: %s\n", optarg);
				return EXIT_FAILURE;
			}
			break;
		case 'C':
			color = 1;
			break;
		}

	// next parameter is the fits filename
	const char	*filename = NULL;
	if (optind < argc) {
		filename = argv[optind++];
	}

	// start OpenCL initialization
	int	err;

	// initialize time measurement
	init_gettime();

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
				"julia1.cl", &err);
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
	cl_kernel	kernel = clCreateKernel(program, "iterate", &err);
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
	size_t	workgroupsize;
	err = clGetKernelWorkGroupInfo(kernel, device_id,
		CL_KERNEL_WORK_GROUP_SIZE,
		sizeof(workgroupsize), &workgroupsize, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: can't get kernel work group info: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "work group size: %lu\n", workgroupsize);
	}

	// allocate the OpenCL memory buffers
	cl_mem	parameters = NULL, output = NULL;

	// we want to have to cluster points
	int	psize = 7;

	// create parameter buffer
	double	*p = (double *)malloc(sizeof(double) * psize);
	parameters = clCreateBuffer(context, CL_MEM_READ_ONLY,
		sizeof(double) * psize, NULL, NULL);
	if (!parameters) {
		fprintf(stderr, "%s:%d: cannot allocate parameter buffer\n",
			__FILE__, __LINE__);
		return EXIT_FAILURE;
	}
	p[0] = origin[0];
	p[1] = origin[1];
	p[2] = size[0] / width;
	p[3] = size[1] / height;
	p[4] = c[0];
	p[5] = c[1];
	p[6] = boundary;

	// create output buffer
	int	imagesize = sizeof(unsigned short) * width * height;
	output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, imagesize,
		NULL, NULL);
	if (!output) {
		fprintf(stderr, "%s:%d: cannot allocate output buffer\n",
			__FILE__, __LINE__);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: buffers allocated\n",
			__FILE__, __LINE__);
	}

	// copy the parameters data to the queue
	err = clEnqueueWriteBuffer(commands, parameters, CL_TRUE, 0,
			sizeof(double) * psize, p, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: cannot enqueue the matrix: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: argument data copied\n",
			__FILE__, __LINE__);
	}

	// set the kernel arguments
	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &parameters);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: cannot set kernel arguments: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: kernel arguments assigned\n",
			__FILE__, __LINE__);
	}

	// compute a suitable work group size
	size_t	global[2] = { width, height };
	size_t	local[2] = { sx, sy };

	// enqueue the kernel
	double	start = gettime();
	err = clEnqueueNDRangeKernel(commands, kernel, 2, NULL, global, local,
		0, NULL, NULL);
	if (err) {
		fprintf(stderr, "%s:%d: cannot enqueue the kernel: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: kernel enqueued\n", __FILE__, __LINE__);
	}

	// read the result data from the queue. This method waits until the
	// the kernel has finished
	unsigned short	*o = (unsigned short *)malloc(imagesize);
	err = clEnqueueReadBuffer(commands, output, CL_TRUE, 0, imagesize, o,
		0, NULL, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: cannot read the data: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	double	end = gettime();
	printf("time: %f\n", end - start);

	// expand if requested
	if (expand) {
		double	min = 65536;
		double	max = 0;
		for (int i = 0; i < width * height; i++) {
			unsigned short	value = o[i];
			if (value > max) {
				max = value;
			}
			if (value < min) {
				min = value;
			}
		}
		for (int i = 0; i < width *  height; i++) {
			o[i] = 65535. * (o[i] - min) / (double)(max - min + 1);
		}
	}

	// write result matrix to a fits file
	if (filename) {
		if (color) {
			write_color(filename, width, height, o, gamma);
		} else {
			write_mono(filename, width, height, o, gamma);
		}
	}

	// perform cleanup
	if (parameters) {
		clReleaseMemObject(parameters);
	}
	if (output) {
		clReleaseMemObject(output);
	}

	// release all the objects
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(commands);
	clReleaseContext(context);

	return EXIT_SUCCESS;
}
