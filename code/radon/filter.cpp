/*
 * filter.cpp -- perform FFT based filtering
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <iostream>
#include <getopt.h>
#include <fitsio.h>
#include <fftw3.h>
#include <math.h>

/**
 * \brief auxiliary function to retrieve the fits error message as std::string
 */
static std::string	fitserr(int status) {
	char	errmsg[80];
	fits_get_errstatus(status, errmsg);
	return std::string(errmsg);
}

/**
 * \brief type of filter to use
 */
typedef enum { DERIVATIVE, SHEPP_LOGAN, RAM_LACK, COSINE } filter_t;

/**
 * \brief Filter selection
 *
 * The filterchoice variable defines which filter the filterfunction below
 * will use. Default is the DERIVATIVE filter.
 */
static filter_t	filterchoice = DERIVATIVE;

/**
 * \brief length parameter in filters
 *
 * All filters but the derivative filter have an additional frequency cut-off
 * paramter L. The default value of this parameter is 1.
 */
static double	L = 1.;

/**
 * \brief Filter function
 *
 * depending on the value of the filterchoice variable, a filter value is
 * returned by which the fourier coefficient is supposed to multiplied.
 */
static double	filterfunction(double s) {
	switch (filterchoice) {
	case DERIVATIVE:
		return s;
	case SHEPP_LOGAN:
		return (s <= L) ? sin(0.5 * M_PI * s / L) : 0.;
	case RAM_LACK:
		return (s <= L) ? (s / L) : 0.;
	case COSINE:
		return (s <= L) ? s * cos(M_PI * s / L) : 0.;
	}
}

/**
 * \brief Perform filtering
 *
 * This method applies an FFT for each row of radon transform, multplies it
 * by the value of the filter function, and transforms it back.
 *
 * \param data	pointer to the data to be filtered
 * \param size	size of the data array 
 */
static int	filter(double *data, int size) {
	fftw_complex	*f = fftw_alloc_complex(size);

	// forward transform to frequency space
	fftw_plan	plan = fftw_plan_dft_r2c_1d(size, data, f, 0);
	fftw_execute(plan);
	fftw_destroy_plan(plan);

	// multiply by filter function
	double	max = 1 + size / 2;
	for (int s = 0; s <= size / 2; s++) {
		double	S = filterfunction(s / max);
		f[s][0] *= S;
		f[s][1] *= S;
	}
	
	// backward transform
	plan = fftw_plan_dft_c2r_1d(size, f, data, 0);
	fftw_execute(plan);
	fftw_destroy_plan(plan);

	// cleanup
	fftw_free(f);

	// return code
	return 0;
}

/**
 * \brief select the filter based on the filter name
 *
 * \param filtername	one of "shepp-logan", "ram-lack", "cosine" or
 *			"derivative"
 */
int	selectfilter(const std::string& filtername) {
	if (filtername == "shepp-logan") {
		filterchoice = SHEPP_LOGAN;
		return 0;
	}
	if (filtername == "ram-lack") {
		filterchoice = RAM_LACK;
		return 0;
	}
	if (filtername == "cosine") {
		filterchoice = COSINE;
		return 0;
	}
	if (filtername == "derivative") {
		filterchoice = DERIVATIVE;
		return 0;
	}
	return -1;
}

/**
 * \brief 
 */
static void	usage(const std::string& progname) {
	std::cout << "usage: " << progname
		<< " [ -h? ] [ -f filter ] [ -l limit ] infile outfile"
		<< std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -f <filter>   use filter named <filter>. Valid filter names are shepp-logan," << std::endl;
	std::cout << "                ram-lack, cosine or derivative"
		<< std::endl;
	std::cout << "  -l <limit>    use <limit> as the high frequency cutoff"
		<< std::endl;
	std::cout << "  -h,-?         display this help message" << std::endl;
}

/**
 * \brief Main function
 */
int	main(int argc, char *argv[]) {

	// command line options for filter specification
	int	c;
	while (EOF != (c = getopt(argc, argv, "f:l:h?")))
		switch (c) {
		case 'f':
			if (selectfilter(optarg) < 0) {
				std::cerr << "unknown filter: " << optarg
					<< std::endl;
				std::cerr << "valid filter names: shepp-logan, "
					"ram-lack, cosine, derivative"
					<< std::endl;
				return EXIT_FAILURE;
			}
			break;
		case 'h':
		case '?':
			usage(argv[0]);
			return EXIT_SUCCESS;
		case 'l':
			L = atof(optarg);
			if ((L <= 0) || (L > 1)) {
				std::cerr << "invalid filter limit: " << L
					<< std::endl;
				return EXIT_FAILURE;
			}
			break;
		}

	// next 2 arguments must be image file names
	if (argc <= (optind + 1)) {
		std::cerr << "not enough arguments: need two image filenames"
			<< std::endl;
		return EXIT_FAILURE;
	}
	char	*infilename = argv[optind++];
	char	*outfilename = argv[optind++];
	
	// read the input file, this is supposed to be a radon transformed image
	int	status = 0;
	fitsfile	*in = NULL;
	if (fits_open_file(&in, infilename, READONLY, &status)) {
		std::cerr << "cannot open fits file: " << fitserr(status)
			<< std::endl;
		return EXIT_FAILURE;
	}

	int	igt;
	int	naxis;
	long	naxes[3];
	if (fits_get_img_param(in, 3, &igt, &naxis, naxes, &status)) {
		std::cerr << "cannot read fits file info: " << fitserr(status)
			<< std::endl;
		return EXIT_FAILURE;
	}

	long	width = naxes[0];
	long	height = naxes[1];
	long	npixels = width * height;
	long	firstpixel[3] = { 1, 1, 1 };
	double	*imagedata = (double *)malloc(npixels * sizeof(double));
	if (fits_read_pix(in, TDOUBLE, firstpixel, npixels, NULL, imagedata,
		NULL, &status)) {
		std::cerr << "cannot read pixel data: " << fitserr(status)
			<< std::endl;
		return EXIT_FAILURE;
	}
	fits_close_file(in, &status);

	// now perform filter on all lines
	for (int row = 0; row < height; row++) {
		filter(imagedata + row * width, width);
	}

	// write the filtered image back to the output file
	fitsfile	*out = NULL;
	if (fits_create_file(&out, outfilename, &status)) {
		std::cerr << "cannot create output FITS file: "
			<< fitserr(status) << std::endl;
		return EXIT_FAILURE;
	}

	naxis = 2;
	if (fits_create_img(out, DOUBLE_IMG, naxis, naxes, &status)) {
		std::cerr << "cannot create output image: " << fitserr(status)
			<< std::endl;
		return EXIT_FAILURE;
	}

	if (fits_write_pix(out, TDOUBLE, firstpixel, npixels, imagedata,
		&status)) {
		std::cerr << "cannot write pixel data: " << fitserr(status)
			<< std::endl;
		return EXIT_FAILURE;
	}

	if (fits_close_file(out, &status)) {
		std::cerr << "cannot close output file: " << fitserr(status)
			<< std::endl;
		return EXIT_FAILURE;
	}

	// cleanup
	free(imagedata);

	return EXIT_SUCCESS;
}
