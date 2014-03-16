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
#include <complex.h>
#include "point.h"

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
 * \brief Compute the initial point
 */
double complex	find_initial(const double complex c) {
	double complex	z = 0.5 + csqrt(0.25 - c);
	for (int i = 0; i < 1000; i++) {
		z = csqrt(z - c);
	}
	return z;
}

/**
 * \brief Main function
 */
int	main(int argc, char *argv[]) {
	// parse command line arguments
	int	gpu = 0;	// whether to use the GPU or the CPU
	int	platform = 0;	// platform number
	int	Debug = 0;
	int	width = 2560;
	int	height = 1440;
	double complex	origin = -2.1 - 1.18125 * I;
	double complex	size = 4.2 + 2.3625 * I;
	double complex	c = -0.52 + 0.57 * I; // default example
	int	initial_iterations = 0;
	int	iterations = 1000;
	int	N = 65535; // number of work items
	int	C;
	int	saturate = 0;
	const char	*histogramfile = NULL;
	while (EOF != (C = getopt(argc, argv, "gdP:Dw:h:H:o:sS:c:n:i:N:")))
		switch (C) {
		case 'd':
			debug = 1;
			break;
		case 'D':
			Debug = 1;
			break;
		case 'g':
			gpu = 1;
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
		case 'H':
			histogramfile = optarg;
			break;
		case 'o':
			if (parse_cpoint(optarg, &origin) < 0) {
				fprintf(stderr, "invalid origin: %s\n", optarg);
				return EXIT_FAILURE;
			}
			break;
		case 's':
			saturate = 1;
			break;
		case 'S':
			if (parse_cpoint(optarg, &size) < 0) {
				fprintf(stderr, "invalid size: %s\n", optarg);
				return EXIT_FAILURE;
			}
			break;
		case 'c':
			if (parse_cpoint(optarg, &c) < 0) {
				fprintf(stderr, "invalid c paramter: %s\n", optarg);
				return EXIT_FAILURE;
			}
			if (debug) {
				fprintf(stderr, "%s:%d: c = %.6f + %.6fi\n",
					__FILE__, __LINE__, creal(c), cimag(c));
			}
			break;
		case 'i':
			initial_iterations = atoi(optarg);
			break;
		case 'n':
			iterations = atoi(optarg);
			break;
		case 'N':
			N = atoi(optarg);
			break;
		}

	// next argument is the output filename
	const char	*filename = NULL;
	if (argc > optind) {
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
				"julia2.cl", &err);
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

	// compute the initial value
	double complex	z = find_initial(c);
	if (debug) {
		fprintf(stderr, "%s:%d: initial point found: %.6f + %.6fi\n",
			__FILE__, __LINE__, creal(z), cimag(z));
	}

	// we want to have to cluster points
	int	psize = 12;

	// create parameter buffer
	double	*p = (double *)malloc(sizeof(double) * psize);
	parameters = clCreateBuffer(context, CL_MEM_READ_ONLY,
		sizeof(double) * psize, NULL, NULL);
	if (!parameters) {
		fprintf(stderr, "%s:%d: cannot allocate parameter buffer\n",
			__FILE__, __LINE__);
		return EXIT_FAILURE;
	}
	p[ 0] = width;
	p[ 1] = height;
	p[ 2] = creal(origin);
	p[ 3] = cimag(origin);
	p[ 4] = creal(size) / width;
	p[ 5] = cimag(size) / height;
	p[ 6] = creal(c);
	p[ 7] = cimag(c);
	p[ 8] = initial_iterations;
	p[ 9] = iterations;
	p[10] = creal(z);
	p[11] = cimag(z);

	// create output buffer, we have to also initialize it to zero and
	// transfer it to the GPU because there is no way to reasonably
	// initialize the memory in the GPU
	int	length = width * height;
	int	imagesize = sizeof(cl_uchar) * length;
	cl_uchar	*o = (cl_uchar *)malloc(imagesize);
	for (int i = 0; i < length; i++) {
		o[i] = 0;
	}
	output = clCreateBuffer(context, CL_MEM_READ_WRITE,
		sizeof(unsigned char) * length, NULL, NULL);
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
		fprintf(stderr, "%s:%d: cannot enqueue the parameters: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	err = clEnqueueWriteBuffer(commands, output, CL_TRUE, 0,
			sizeof(cl_uchar) * length, o, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: cannot enqueue the image: %d\n",
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
	size_t	local = workgroupsize;

	// find the smallest multiple of the workgroupsize that is larger
	// than N
	size_t	global = workgroupsize * ((N / workgroupsize) + ((N % workgroupsize) ? 1 : 0));
	if (debug) {
		fprintf(stderr, "%s:%d: using local size = %ld, "
			"global size = %ld\n", __FILE__, __LINE__,
			local, global);
	}

	// enqueue the kernel
	double	start = gettime();
	err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local,
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
	err = clEnqueueReadBuffer(commands, output, CL_TRUE, 0, imagesize, o,
		0, NULL, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "%s:%d: cannot read the data: %d\n",
			__FILE__, __LINE__, err);
		return EXIT_FAILURE;
	}
	double	end = gettime();
	printf("time: %d,%d,%f,%f,%f\n", width, height, creal(c), cimag(c),
		end - start);

	// compute histogramm of values
	if (histogramfile) {
		unsigned long	histogramm[256];
		for (int i = 0; i < 256; i++) { histogramm[i] = 0; }
		for (int i = 0; i < width * height; i++) {
			histogramm[o[i]]++;
		}
		FILE	*histogram = fopen(histogramfile, "w");
		if (NULL == histogram) {
			fprintf(stderr, "cannot write histogram to %s: %s\n",
				histogramfile, strerror(errno));
		} else {
			fprintf(histogram, "v,count\n");
			for (int i = 0; i < 256; i++) {
				fprintf(histogram, "%d,%ld\n", i, histogramm[i]);
			}
			fclose(histogram);
		}
	}

	// saturate the image if requested
	if (saturate) {
		if (debug) {
			fprintf(stderr, "%s:%d: saturating image\n",
				__FILE__, __LINE__);
		}
		for (int i = 0; i < width * height; i++) {
			if (o[i] > 0) {
				o[i] = 255;
			}
		}
	}

	// write the result array as an image
	if (filename) {
		fitsfile	*fits = NULL;
		int	status = 0;
		char	errmsg[80];
		unlink(filename);
		if (fits_create_file(&fits, filename, &status)) {
			fits_get_errstatus(status, errmsg);
			fprintf(stderr, "cannot create FITS file %s: %s\n",
				filename, errmsg);
			goto cleanup;
		}

		int	naxis = 2;
		long	naxes[2] = { width, height };
		if (fits_create_img(fits, BYTE_IMG, naxis, naxes, &status)) {
			fits_get_errstatus(status, errmsg);
			fprintf(stderr, "cannot create image: %s\n", errmsg);
			goto cleanup;
		}

		long	npixels = width * height;
		long	firstpixel[2] = { 1, 1 };
		if (fits_write_pix(fits, TBYTE, firstpixel, npixels, o, &status)) {
			fits_get_errstatus(status, errmsg);
			fprintf(stderr, "cannot write pixels: %s\n", errmsg);
			goto cleanup;
		}
	cleanup:
		status = 0;
		if (fits) {
			if (fits_close_file(fits, &status)) {
				fits_get_errstatus(status, errmsg);
				fprintf(stderr, "cannot close file: %s\n",
					errmsg);
			}
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
