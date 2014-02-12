/*
 * ncwrite.c -- sample program showing how to write netcdf files
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <netcdf.h>
#include <complex.h>
#include <math.h>

const double	originx = -0.656;
const double	originy = 0.479;
const double	sizex = 0.14;
const double	sizey = 0.1;

/**
 * \brief Perform the simulation we want to record in the NetCDF file
 */
void	simulation(double *data, int width, int height, int t) {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			double complex	c = originx + x * sizex / width
				+ (originy + y * sizey / height) * I;
			double	complex	z = c;
			for (int i = 0; i < t; i++) {
				z = z * z + c;
			}
			data[x + y * width] = log(log(cabs(z)));
		}
	}
}

int	main(int argc, char *argv[]) {
	// parse command line options
	int	c;
	while (EOF != (c = getopt(argc, argv, "")))
		switch (c) {
		}

	// use the next argument as file name
	if (optind >= argc) {
		fprintf(stderr, "file name argument missing\n");
		return EXIT_FAILURE;
	}
	char	*filename = argv[optind];

	// create the file
	int	status;
	int	fileid;
	if (NC_NOERR != (status = nc_create(filename, NC_NOCLOBBER, &fileid))) {
		fprintf(stderr, "cannot create file %s: %s\n",
			filename, nc_strerror(status));
		return EXIT_FAILURE;
	}

	// define definitions of the array
	size_t	lenx = 140, leny = 100;
	int	x_dim, y_dim, t_dim; // dimension ids
	if (NC_NOERR != (status = nc_def_dim(fileid, "x", lenx, &x_dim))) {
		fprintf(stderr, "%s:%d: cannot define x dimension: %s\n",
			__FILE__, __LINE__, nc_strerror(status));
		return EXIT_FAILURE;
	}
	if (NC_NOERR != (status = nc_def_dim(fileid, "y", leny, &y_dim))) {
		fprintf(stderr, "%s:%d: cannot define y dimension: %s\n",
			__FILE__, __LINE__, nc_strerror(status));
		return EXIT_FAILURE;
	}
	if (NC_NOERR != (status = nc_def_dim(fileid, "t", NC_UNLIMITED,
		&t_dim))) {
		fprintf(stderr, "%s:%d: cannot define t dimension: %s\n",
			__FILE__, __LINE__, nc_strerror(status));
		return EXIT_FAILURE;
	}

	// define the array
	int	arrayid;
	int	dimensions[3] = { t_dim, x_dim, y_dim };
	if (NC_NOERR != (status = nc_def_var(fileid, "results", NC_DOUBLE,
		3, dimensions, &arrayid))) {
		fprintf(stderr, "%s:%d: cannot define t dimension: %s\n",
			__FILE__, __LINE__, nc_strerror(status));
		return EXIT_FAILURE;
	}

	// end define mode
	if (NC_NOERR != (status = nc_enddef(fileid))) {
		fprintf(stderr, "%s:%d: cannot end define mode: %s\n",
			__FILE__, __LINE__, nc_strerror(status));
		return EXIT_FAILURE;
	}

	// now we have to do the simulation
	double	*data = (double *)malloc(lenx * leny * sizeof(double));
	for (int t = 0; t < 30; t++) {
		// perform the simulation
		simulation(data, lenx, leny, t);
	
		// add the data to the NetCDF file
		size_t	start[3] = { t, 0, 0 };
		size_t	size[3] = { 1, lenx, leny };
		status = nc_put_vara(fileid, arrayid, start, size, data);
		if (NC_NOERR != status) {
			fprintf(stderr, "cannot write image: %s\n",
				nc_strerror(status));
			return EXIT_FAILURE;
		}
	}

	// close the file
	if (NC_NOERR != nc_close(fileid)) {
		fprintf(stderr, "%s:%d: cannot close file: %s\n",
			__FILE__, __LINE__, nc_strerror(status));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
