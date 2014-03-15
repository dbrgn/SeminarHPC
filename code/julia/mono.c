/*
 * mono.c -- write a mono image
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include "mono.h"
#include <fitsio.h>
#include <unistd.h>

int	write_mono(const char *filename, const int width,
			const int height, const unsigned short *pixels) {
	int	rc = 0;
	fitsfile	*fits = NULL;
	int	status = 0;
	char	errmsg[80];
	unlink(filename);
	if (fits_create_file(&fits, filename, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot create FITS file %s: %s\n",
			filename, errmsg);
		rc = -1;
		goto cleanup;
	}

	int	naxis = 2;
	long	naxes[2] = { width, height };
	if (fits_create_img(fits, SHORT_IMG, naxis,  naxes, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot create image: %s\n", errmsg);
		rc = -1;
		goto cleanup;
	}

	long	npixels = width * height;
	long	firstpixel[2] = { 1, 1 };
	if (fits_write_pix(fits, TUSHORT, firstpixel, npixels, pixels, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "write pixel data: %s\n", errmsg);
		rc = -1;
		goto cleanup;
	}

cleanup:
	if (fits) {
		status = 0;
		if (fits_close_file(fits, &status)) {
			fits_get_errstatus(status, errmsg);
			fprintf(stderr, "cannot close file: %s\n",
				errmsg);
		}
	}
	return rc;
}
