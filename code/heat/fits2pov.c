/*
 * fits2pov.c -- convert FITS file to povray structure
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#define _SVID_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <fitsio.h>
#include "utils.h"

int	debug = 0;

/**
 * \brief Display a usage message
 */
void	usage(const char *progname) {
	printf("usage: %s fitsfile povrayfile\n", progname);
	printf("convert a FITS file into a povray structure\n");
	printf("options:\n");
	printf(" -a           preserve aspect ratio\n");
	printf(" -d           increase debug level\n");
	printf(" -h,-?        display this help message and exit\n");
	printf(" -x <hx>      set x-axis step size to <hx>\n");
	printf(" -y <hy>      set y-axis step size to <hy>\n");
	printf(" -s <scale>   set vertical scale factor to <scale>\n");
}

/**
 * \brief Output a point in povray format
 */
void	point(FILE *outfile, const double *p) {
	fprintf(outfile, "<%.4f, %.4f, %.4f>", p[0], p[2], p[1]);
}

/**
 * \brief Output a triangle in povray format
 */
void	triangle(FILE *outfile, const double *t) {
	fprintf(outfile, "triangle { ");
	point(outfile, &t[0]);
	fprintf(outfile, ", ");
	point(outfile, &t[3]);
	fprintf(outfile, ", ");
	point(outfile, &t[6]);
	fprintf(outfile, " }\n");
}

/**
 * \brief main function for the fits2pov program
 */
int	main(int argc, char *argv[]) {
	FILE	*outfile = NULL;
	int	rc = EXIT_FAILURE;
	int	c;
	int	preserve_aspect = 0;
	double	scale = 1;
	double	hx = -1;
	double	hy = -1;
	while (EOF != (c = getopt(argc, argv, "adh?s:x:y:")))
		switch (c) {
		case 'a':
			preserve_aspect = 1;
			break;
		case 'd':
			debug++;
			break;
		case 'h':
		case '?':
			usage(argv[0]);
			return EXIT_SUCCESS;
		case 'x':
			hx = atof(optarg);
			break;
		case 'y':
			hy = atof(optarg);
			break;
		case 's':
			scale = atof(optarg);
			break;
		}

	// next two arguments must be the file names
	if (optind >= argc) {
		fprintf(stderr, "missing file name argument\n");
		return EXIT_FAILURE;
	}
	char	*fitsname = argv[optind++];
	if (debug) {
		fprintf(stderr, "%s:%d: FITS file: %s\n",
			__FILE__, __LINE__, fitsname);
	}

	// get the povray file name
	char	*povname = NULL;
	if (optind < argc) {
		povname = strdup(argv[optind++]);
	} else {
		povname = strdup(fitsname);
		// replace .fits extension by .inc
		char	*where = strstr(povname, ".fits");
		if (where) {
			strcpy(where, ".inc");
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d: convert %s to %s\n",
			__FILE__, __LINE__, fitsname, povname);
	}

	// read the image
	image_t	*image = readfits(fitsname);
	if (NULL == image) {
		goto cleanup;
	}

	// complete the dimensions
	if (hx < 0) {
		hx = 1. / image->width;
	}
	if (hy < 0) {
		hy = 1. / image->height;
	}
	if (preserve_aspect) {
		if (hx > hy) {
			hx = hy;
		} else {
			hy = hx;
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d: hx = %f, hy = %f\n", __FILE__, __LINE__,
			hx, hy);
	}

	// open the output file
	outfile = fopen(povname, "w");
	if (NULL == outfile) {
		fprintf(stderr, "cannot open file %s: %s\n", povname,
			strerror(errno));
		goto cleanup;
	}

	// open the file
	fprintf(outfile, "mesh {\n");

	// write the data as a povray structure
	for (int x = 0; x < image->width - 1; x++) {
		for (int y = 0; y < image->height - 1; y++) {
			double	t[9];
			t[0] = x * hx;
			t[1] = y * hy;
			t[2] = scale * value(image, x, y);
			t[3] = (x + 1) * hx;
			t[4] = y * hy;
			t[5] = scale * value(image, x + 1, y); 
			t[6] = x * hx;
			t[7] = (y + 1) * hy;
			t[8] = scale * value(image, x, y + 1);
			triangle(outfile, t);
			t[0] = (x + 1) * hx;
			t[1] = (y + 1) * hy;
			t[2] = scale * value(image, x + 1, y + 1);
			triangle(outfile, t);
			if (debug > 1) {
				double	v = scale * value(image, x, y);
				fprintf(stderr, "data[%.4f,%.4f] = %16.12f\n",
					x * hx, y * hy, v);
			}
		}
	}

	fprintf(outfile, "pigment { color White }\n");
	fprintf(outfile, "}\n");

	// cleanup
	rc = EXIT_SUCCESS;
	
cleanup:
	// close the output file
	if (outfile) {
		fclose(outfile);
		outfile = NULL;
	}

	if (image) {
		freeimage(image);
		image = NULL;
	}
	free(povname); povname = NULL;

	return rc;
}
