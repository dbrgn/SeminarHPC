/*
 * color.c -- write an array as a color image
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include "color.h"
#include "fitsio.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>

extern int debug;

static void	make_color(unsigned char *color, const double value) {
	int	h = floor(value / 0.1666666666);
	double	f = (value / 0.166666666) - h;
	double	S = 1;
	double	V = 255 * 1;
	double	p = V * (1 - S);
	double	q = V * (1 - S * f);
	double	t = V * (1 - S * (1 - f));
	switch (h) {
	case 0:
	case 6:	color[0] = V;
		color[1] = t;
		color[2] = p;
		break;
	case 1: color[0] = q;
		color[1] = V;
		color[2] = p;
		break;
	case 2: color[0] = p;
		color[1] = V;
		color[2] = t;
		break;
	case 3: color[0] = p;
		color[1] = q;
		color[2] = V;
		break;
	case 4: color[0] = t;
		color[1] = p;
		color[2] = V;
		break;
	case 5: color[0] = V;
		color[1] = p;
		color[2] = q;
		break;
	}
//printf("%10.6f %3d %3d %3d\n", value, (int)color[0], (int)color[1], (int)color[2]);
}

int	write_color(const char *filename, const int width, const int height,
			const unsigned short *pixels) {
	int	rc = 0;
	if (debug) {
		fprintf(stderr, "%s:%d: writing %d x %d color image to %s (%p)\n",
			__FILE__, __LINE__, width, height, filename, pixels);
	}

	// first find the maximum and minimum levels
	int	max = 0;
	int	min = 65536;
	long	fieldsize = width * height;
	for (int i = 0; i < fieldsize; i++) {
		unsigned short	v = pixels[i];
		if (v < min) {
			min = v;
		}
		if (v > max) {
			max = v;
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d: max = %d, min = %d\n",
			__FILE__, __LINE__, max, min);
	}

	// allocate a data array for the RGB pixel information
	long	npixels = fieldsize * 3;
	unsigned char	*data = (unsigned char *)malloc(npixels);
	if (NULL == data) {
		fprintf(stderr, "cannot allocate memory for color image: %s\n",
			strerror(errno));
		return -1;
	}

	// convert the pixel values to RGB
	for (int i = 0; i < fieldsize; i++) {
		double	value = (pixels[i] - min) / (double)(max - min + 1);
		unsigned char	color[3] = { 0, 0, 0 };
		make_color(color, value);
		data[i                ] = color[0];
		data[i +     fieldsize] = color[1];
		data[i + 2 * fieldsize] = color[2];
	}
	if (debug) {
		fprintf(stderr, "%s:%d: converted to color\n",
			__FILE__, __LINE__);
	}

	// write the array to a fits file
	int	status = 0;
	fitsfile	*fits = NULL;
	char	errmsg[80];
	unlink(filename);
	if (fits_create_file(&fits, filename, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot create FITS file %s: %s\n",
			filename, errmsg);
		rc = -1;
	}

	int	naxis = 3;
	long	naxes[3] = { width, height, 3 };
	if (fits_create_img(fits, BYTE_IMG, naxis, naxes, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot create FITS image: %s\n", errmsg);
		rc = -1;
	}

	long	firstpixel[3] = { 1, 1, 1 };
	if (fits_write_pix(fits, TBYTE, firstpixel, npixels, data, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot write pixel data: %s\n", errmsg);
		rc = -1;
	}

	if (fits_close_file(fits, &status)) {
		fits_get_errstatus(status, errmsg);
		fprintf(stderr, "cannot close FITS file: %s\n", errmsg);
		rc = -1;
	}

	// free data array
	free(data);

	// that's it
	return rc;
}
