/*
 * utils.c -- some common functions
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <fitsio.h>
#include <math.h>
#include <errno.h>
#include <string.h>

extern int debug;

/**
 * \brief get image size
 */
long	npixels(const image_t *image) {
	return image->width * image->height;
}

/**
 * \brief free an image
 */
void	freeimage(image_t *image) {
	if (NULL == image) {
		return;
	}
	if (NULL != image->data) {
		free(image->data);
		image->data = NULL;
	}
	free(image);
}

/**
 * \brief Open a FITS file and read the data into a buffer
 *
 * \param fitsname	name of the FITS file to open
 * \param width		pointer to variable holding the width of the image
 * \param height	pointer to variable holding the height of the image
 * \return		double array containing FITS file content data
 */
image_t	*readfits(const char *fitsname) {
	// allocate a new image structure
	image_t	*image = (image_t *)malloc(sizeof(image_t));
	if (image == NULL) {
		goto cleanup;
	}
	image->data = NULL;

	// read the FITS file
	char	fitserrmsg[80];
	int	status = 0;
	fitsfile	*fits = NULL;
	if (fits_open_file(&fits, fitsname, READONLY, &status)) {
		fits_get_errstatus(status, fitserrmsg);
		fprintf(stderr, "cannot open file %s: %s\n", fitsname,
			fitserrmsg);
		goto cleanup;
	}

	int	igt;
	int	naxis;
	long	naxes[3];
	if (fits_get_img_param(fits, 3, &igt, &naxis, naxes, &status)) {
		fits_get_errstatus(status, fitserrmsg);
		fprintf(stderr, "cannot read file info: %s\n", fitserrmsg);
		freeimage(image);
		image = NULL;
		goto cleanup;
	}

	image->width = naxes[0];
	image->height = naxes[1];
	if (debug) {
		fprintf(stderr, "%s:%d: got %ld x %ld image\n",
			__FILE__, __LINE__,  image->width, image->height);
	}

	long	firstpixel[3] = { 1, 1, 1 };
	image->data = (double *)malloc(npixels(image) * sizeof(double));
	if (NULL == image->data) {
		fprintf(stderr, "cannot allocate pixel array: %s\n",
			strerror(errno));
		freeimage(image);
		image = NULL;
		goto cleanup;
	}
	if (debug) {
		fprintf(stderr, "%s:%d: allocated %ld doubles\n",
			__FILE__, __LINE__, npixels(image));
	}
	if (fits_read_pix(fits, TDOUBLE, firstpixel, npixels(image), NULL,
		image->data, NULL, &status)) {
		fits_get_errstatus(status, fitserrmsg);
		fprintf(stderr, "cannot read pixel data: %s\n", fitserrmsg);
		freeimage(image);
		image = NULL;
	}

cleanup:
	if (fits) {
		status = 0;
		if (fits_close_file(fits, &status)) {
			fits_get_errstatus(status, fitserrmsg);
			fprintf(stderr, "cannot close FITS file: %s\n",
				fitserrmsg);
		}
	}
	return image;
}

/**
 * \brief find maximum absolute value of an array
 *
 * \param data		array containing the data
 * \param npixels	number of pixels to examine
 * \return		maximum absolute value
 */
double	datamax(const image_t *image) {
	double	m = 0;
	long	n = npixels(image);
	for (long i = 0; i < n; i++) {
		double	l = fabs(image->data[i]);
		if (l > m) {
			m = l;
		}
	}
	return m;
}

/**
 * \brief Access pixels
 */
double	value(const image_t *image, long x, long y) {
	return image->data[x + y * image->width];
}
