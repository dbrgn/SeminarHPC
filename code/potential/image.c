/*
 * image.c -- image read/write routines
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <fitsio.h>
#include "image.h"

extern int	debug;

image_t	*readimage(const char *filename) {
	char	errmsg[80];
	image_t	*image = NULL;
	// open the FITS file
	int	status = 0;
	fitsfile	*fits = NULL;
	if (fits_open_file(&fits, filename, READONLY, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot read fits file %s: %s\n",
			filename, errmsg);
		return NULL;
	}

	// find dimensions of pixel array
	int	igt;
	int	naxis;
	long	naxes[3];
	if (fits_get_img_param(fits, 3, &igt, &naxis, naxes, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot read fits file info: %s\n", errmsg);
		goto bad;
	}
	image = (image_t *)malloc(sizeof(image_t));
	image->width = naxes[0];
	image->height = naxes[1];

	// read the pixel data
	long	npixels = image->width * image->height;
	image->data = (double *)malloc(npixels * sizeof(double));
	long	firstpixel[3] = { 1, 1, 1 };
	if (fits_read_pix(fits, TDOUBLE, firstpixel, npixels, NULL, image->data,
		NULL, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot read pixel data: %s\n", errmsg);
		goto bad;
	}

	// close the file
	if (fits_close_file(fits, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot close fits file %s: %s\n",
			filename, errmsg);
		goto bad;
	}

	// return the image
	return image;
bad:
	if (image) {
		if (image->data) {
			free(image->data);
		}
		free(image);
	}
	if (fits) {
		fits_close_file(fits, &status);
	}
	return NULL;
}

void	writeimage(const image_t *image, const char *filename) {
	if (debug) {
		fprintf(stderr, "%s:%d: writing image %s\n", __FILE__, __LINE__,
			filename);
	}
	char	errmsg[80];
	int	status = 0;
	fitsfile	*fits = NULL;
	if (fits_create_file(&fits, filename, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot create fits file %s: %s\n",
			filename, errmsg);
		return;
	}

	// find dimensions of pixel array
	int	naxis = 2;
	long	naxes[2] = { image->width, image->height };
	if (fits_create_img(fits, DOUBLE_IMG, naxis, naxes, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot create image: %s\n", errmsg);
		goto bad;
	}

	// read the pixel data
	long	npixels = image->width * image->height;
	long	firstpixel[3] = { 1, 1, 1 };
	if (fits_write_pix(fits, TDOUBLE, firstpixel, npixels,
			image->data, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot write pixel data: %s\n", errmsg);
	}

bad:
	// close the file
	if (fits_close_file(fits, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot close fits file %s: %s\n",
			filename, errmsg);
		goto bad;
	}
}
