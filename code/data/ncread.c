/*
 * ncread.c -- sample program showing how to read netcdf files, and writing
 *             the contents to FITS files
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <netcdf.h>
#include <fitsio.h>

int	main(int argc, char *argv[]) {

	// parse the command line options, if any
	int	c;
	while (EOF != (c = getopt(argc, argv, "")))
		switch (c) {
		}

	// get the filename
	if (optind >= argc) {
		fprintf(stderr, "file name argument missing\n");
		return EXIT_FAILURE;
	}
	char	*filename = argv[optind];
	printf("opening file %s\n", filename);

	// read a netcdf file
	int	status;
	int	fileid;
	if (NC_NOERR != (status = nc_open(filename, NC_NOWRITE, &fileid))) {
		fprintf(stderr, "cannot open file: %s\n", nc_strerror(status));
		return EXIT_FAILURE;
	}
	printf("fileid: %d\n", fileid);

	// find the variable named
	int	varid;
	if (NC_NOERR != (status = nc_inq_varid(fileid, "results", &varid))) {
		fprintf(stderr, "cannot find \"results\" variable: %s\n",
			nc_strerror(status));
		return EXIT_FAILURE;
	}
	printf("varid: %d\n", varid);

	// use the nc_inq_var method to find information about the 
	// variable. Note that nc_inq_vardimid does not work so we must
	// use this function.
	nc_type	type;
	int	ndims;
	int	dimid[3];
	if (NC_NOERR != (status = nc_inq_var(fileid, varid, NULL, &type,
		&ndims, dimid, NULL))) {
		fprintf(stderr, "cannot get variable info: %s\n",
			nc_strerror(status));
		return EXIT_FAILURE;
	}

	printf("number of dimensions: %d\n", ndims);
	printf("dimids: %d, %d, %d\n", dimid[0], dimid[1], dimid[2]);

	// get information about each dimension id
	size_t	lenx, leny, lent;
	nc_inq_dimlen(fileid, dimid[0], &lent);
	nc_inq_dimlen(fileid, dimid[1], &lenx);
	nc_inq_dimlen(fileid, dimid[2], &leny);
	printf("sizes: %ld, %ld, %ld\n", lent, lenx, leny);

	// find out about how many planes there are
	double	*data = (double *)malloc(lenx * leny * sizeof(double));
	size_t	start[3] = { 0, 0, 0 };
	size_t	count[3] = { 1, lenx, leny };
	for (int t = 0; t < lent; t++) {
		start[0] = t;
		status = nc_get_vara_double(fileid, varid, start, count, data);
		if (status != NC_NOERR) {
			fprintf(stderr, "could not read image %d: %s\n", t,
				nc_strerror(status));
			return EXIT_FAILURE;
		}

		// get the file name
		char	fitsname[1024];
		snprintf(fitsname, sizeof(fitsname), "!images/f%03d.fits", t);

		// create a fits file
		fitsfile	*fits;
		status = 0;
		if (fits_create_file(&fits, fitsname, &status)) {
			fprintf(stderr, "cannot create fits file %s: %d\n",
				fitsname, status);
			return EXIT_FAILURE;
		}

		// create the image
		long	naxis = 2;
		long	naxes[2] = { lenx, leny };
		if (fits_create_img(fits, DOUBLE_IMG, naxis, naxes, &status)) {
			fprintf(stderr, "cannot create FITS image: %d\n",
				status);
			return EXIT_FAILURE;
		}

		// write pixel data
		long	fpixel[2] = { 1, 1 };
		if (fits_write_pix(fits, TDOUBLE, fpixel, lenx * leny, data,
			&status)) {
			fprintf(stderr, "cannot write pixel data: %d\n",
				status);
			return EXIT_FAILURE;
		}

		// close the fits file
		if (fits_close_file(fits, &status)) {
			fprintf(stderr, "cannot close FITS file '%s': %d\n",
				fitsname, status);
			return EXIT_FAILURE;
		}
	}

	// we are done
	return EXIT_SUCCESS;
}
