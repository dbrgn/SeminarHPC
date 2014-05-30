/*
 * fits2off.c -- convert FITS file to object file
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

#define	XSKIRT	1
#define	YSKIRT	1
#define	BOTTOM	1

int	debug = 0;

/**
 * \brief Display usage message for the program
 */
void	usage(const char *progname) {
	printf("usage: %s fitsfile povrayfile\n", progname);
	printf("convert a FITS file into a povray structure\n");
	printf("options:\n");
	printf(" -a           preserve aspect ratio\n");
	printf(" -d           enable debug mode\n");
	printf(" -x hx        set x-axis step size\n");
	printf(" -y hy        set y-axis step size\n");
	printf(" -s scale     set z-axis scale factor\n");
	printf(" -m maximum   scale the data so that the maximum is <maximum>\n");
	printf("              incompatible with -s\n");
	printf(" -o offset    add offset <offset> to all values\n");
	printf(" -h,-?        show this help\n");
}

/**
 * \brief Main function of the fits2off program
 */
int	main(int argc, char *argv[]) {
	double	scale = 1;	/* scale factor to apply to all values */
	double	maximum = -1;	/* maximum absolute value */
	double	offset = 0;	/* offset to add to all values after scaling */
	double	hx = -1;	/* step size in x-direction */
	double	hy = -1;	/* step size in y-direction */
	FILE	*outfile = NULL;
	int	rc = EXIT_FAILURE;
	int	c;
	int	preserve_aspect = 0;
	while (EOF != (c = getopt(argc, argv, "adh?s:x:y:m:o:")))
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
		case 'm':
			maximum = atof(optarg);
			break;
		case 'o':
			offset = atof(optarg);
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
	char	*offname = NULL;
	if (optind < argc) {
		offname = strdup(argv[optind++]);
	} else {
		offname = strdup(fitsname);
		// replace .fits extension by .off
		char	*where = strstr(offname, ".fits");
		if (where) {
			strcpy(where, ".off");
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d: convert %s to %s\n",
			__FILE__, __LINE__, fitsname, offname);
	}

	// read the image file
	image_t	*image = readfits(fitsname);
	if (NULL == image) {
		goto cleanup;
	}

	// complete the scaling information
	// by default, scale the image to a square of side 1
	if (hx < 0) {
		hx = 1. / image->width;
	}
	if (hy < 0) {
		hy = 1. / image->height;
	}
	// if the -a option is given, set the scale factors so that the
	// image rectangle fits inside a square of side 1
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

	// if the maximum is set, we want to modify the scale so that
	// we get the right maximum
	if (maximum > 0) {
		scale = maximum / datamax(image);
	}

	// open the output file
	outfile = fopen(offname, "w");
	if (NULL == outfile) {
		fprintf(stderr, "cannot open file %s: %s\n", offname,
			strerror(errno));
		goto cleanup;
	}

	// write the header of the file, i. e. number of points and number
	// of triangles
	fprintf(outfile, "OFF\n%ld %ld 0\n\n",
		image->width * image->height
		+ 2 * image->width + 2 * (image->height - 2),
		2 * (image->width - 1) * (image->height - 1)
#if XSKIRT
		+ 4 * (image->width - 1)
#endif /* XSKIRT */
#if YSKIRT
		+ 4 * (image->height - 1)
#endif /* YSKIRT */
#if BOTTOM
		+ 2
#endif /* BOTTOM */
		);

	long	pointcounter = 0;
#define	addpoint(X, Y, Z)						\
	do {								\
		if (debug > 1) {					\
			fprintf(stderr, "%s:%d: %ld (%f, %f, %f)\n",\
				__FILE__, __LINE__, pointcounter,	\
				X, Y, Z);				\
		}							\
		fprintf(outfile, "%g %g %g\n",				\
			X, Y, Z);					\
		pointcounter++;						\
	} while (0)

	// write all points on individual lines
	for (long y = 0; y < image->height; y++) {
		for (long x = 0; x < image->width; x++) {
			addpoint(x * hx, y * hy,
				scale * value(image, x, y) + offset);
		}
	}

	// add points at bottom, along x-axis
	if (debug) {
		fprintf(stderr, "%s:%d: xaxis points start at: %ld\n",
			__FILE__, __LINE__, pointcounter);
	}
	long	xborder = pointcounter;
	for (long x = 0; x < image->width; x++) {
		addpoint(x * hx, 0., 0.);
		addpoint(x * hx, (image->height - 1) * hy, 0.);
	}

	// add points at bottom, along y-axis
	if (debug) {
		fprintf(stderr, "%s:%d: yaxis points start at: %ld\n",
			__FILE__, __LINE__, pointcounter);
	}
	long	yborder = pointcounter;
	for (long y = 1; y < image->height - 1; y++) {
		addpoint(0., y * hy, 0.);
		addpoint((image->width - 1) * hx, y * hy, 0.);
	}
	if (debug) {
		fprintf(stderr, "%s:%d: points: %ld\n", __FILE__, __LINE__,
			pointcounter);
	}

	// start adding triangles
	long	trianglecounter = 0;

#define	addquad(a, b, c, d)						\
	do {								\
		if (debug > 1) {					\
			fprintf(stderr, "%s:%d: %ld (%ld %ld %ld)\n",	\
				__FILE__, __LINE__,			\
				trianglecounter, a, b, c);		\
		}							\
		fprintf(outfile, "3 %ld %ld %ld\n", a, b, c);		\
		trianglecounter++;					\
		if (debug > 1) {					\
			fprintf(stderr, "%s:%d: %ld (%ld %ld %ld)\n",	\
				__FILE__, __LINE__,			\
				trianglecounter, a, c, d);		\
		}							\
		fprintf(outfile, "3 %ld %ld %ld\n", a, c, d);		\
		trianglecounter++;					\
	} while (0)

	// write triangles of the surface
	if (debug) {
		fprintf(stderr, "%s:%d: add surface triangles\n",
			__FILE__, __LINE__);
	}
	for (long y = 0; y < image->height - 1; y++) {
		for (long x = 0; x < image->width - 1; x++) {
			addquad((x    ) + image->width * (y    ),
				(x + 1) + image->width * (y    ),
				(x + 1) + image->width * (y + 1),
				(x    ) + image->width * (y + 1));
		}
	}

	// write triangles of the boundaries
#if XSKIRT
	if (debug) {
		fprintf(stderr, "%s:%d: add X-skirt triangles\n",
			__FILE__, __LINE__);
	}
	for (long x = 0; x < image->width - 1; x++) {
		addquad(xborder + 2 * x,
			xborder + 2 * x + 2,
			x + 1,
			x);
		addquad(xborder + 2 * x + 1,
			x + (image->height - 1) * image->width,
			x + (image->height - 1) * image->width + 1,
			xborder + 2 * x + 3);
	}
#endif /* XSKIRT */
#if YSKIRT
	if (debug) {
		fprintf(stderr, "%s:%d: adding Y-Skirt triangles\n",
			__FILE__, __LINE__);
	}
	for (long y = 1; y < image->height - 2; y++) {
		addquad(yborder + 2 * (y - 1),
			y * image->width,
			(y + 1) * image->width,
			yborder + 2 * y);
		addquad(yborder + 2 * y + 1,
			(y + 1) * image->width + image->width - 1,
			y * image->width + image->width - 1,
			yborder + 2 * (y - 1) + 1);
	}
	if (debug) {
		fprintf(stderr, "%s:%d: add exceptional quads\n",
			__FILE__, __LINE__);
	}
	addquad(xborder,
		(long)0,
		image->width,
		yborder);

	addquad(yborder + 1,
		2 * image->width - 1,
		image->width - 1,
		xborder + 2 * (image->width - 1));

	addquad(yborder + 2 * (image->height - 3),
		(image->height - 2) * image->width,
		(image->height - 1) * image->width,
		xborder + 1);

	addquad(xborder + 2 * image->width - 1,
		image->height * image->width - 1,
		(image->height - 1) * image->width - 1,
		yborder + 2 * (image->height - 3) + 1);
		
#endif /* YSKIRT */

	// write triangles of the bottom
#if BOTTOM
	if (debug) {
		fprintf(stderr, "%s:%d: add bottom\n", __FILE__, __LINE__);
	}
	addquad(xborder,
		xborder + 1,
		xborder + 2 * image->width - 1,
		xborder + 2 * image->width - 2);
#endif
		
	// ensure output file ends with a blank line
	fprintf(outfile, "\n");

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
	free(offname); offname = NULL;

	return rc;
}
