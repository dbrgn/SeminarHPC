/*
 * fits2off.c -- convert FITS file to object file
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <fitsio.h>

#define	XSKIRT	1
#define	YSKIRT	1
#define	BOTTOM	1

int	debug = 0;
double	*data = NULL;
double	scale = 1;
double	maximum = -1;
double	offset = 0;
double	hx = -1;
double	hy = -1;

void	usage(const char *progname) {
	printf("usage: %s fitsfile povrayfile\n", progname);
	printf("convert a FITS file into a povray structure\n");
}

void	point(FILE *outfile, const double *p) {
	fprintf(outfile, "<%.4f, %.4f, %.4f>", p[0], p[2], p[1]);
}

void	triangle(FILE *outfile, const double *t) {
	fprintf(outfile, "triangle { ");
	point(outfile, &t[0]);
	fprintf(outfile, ", ");
	point(outfile, &t[3]);
	fprintf(outfile, ", ");
	point(outfile, &t[6]);
	fprintf(outfile, " }\n");
}

int	main(int argc, char *argv[]) {
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
		goto cleanup;
	}

	long	width = naxes[0];
	long	height = naxes[1];
	if (debug) {
		fprintf(stderr, "%s:%d: got %ld x %ld image\n",
			__FILE__, __LINE__,  width, height);
	}

	long	firstpixel[3] = { 1, 1, 1 };
	long	npixels = width * height;
	data = (double *)malloc(npixels * sizeof(double));
	if (debug) {
		fprintf(stderr, "%s:%d: allocated %ld doubles\n",
			__FILE__, __LINE__, npixels);
	}
	if (fits_read_pix(fits, TDOUBLE, firstpixel, npixels, NULL, data,
		NULL, &status)) {
		fits_get_errstatus(status, fitserrmsg);
		fprintf(stderr, "cannot read pixel data: %s\n", fitserrmsg);
		goto cleanup;
	}

	if (fits_close_file(fits, &status)) {
		fits_get_errstatus(status, fitserrmsg);
		fprintf(stderr, "cannot close FITS file: %s\n", fitserrmsg);
		goto cleanup;
	}

	// complete the dimensions
	if (hx < 0) {
		hx = 1. / width;
	}
	if (hy < 0) {
		hy = 1. / height;
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

	// if the maximum is set, we want to modify the scale so that
	// we get the right maximum
	if (maximum > 0) {
		double	m = 0;
		for (long i = 0; i < width * height; i++) {
			if (data[i] > m) {
				m = data[i];
			}
		}
		scale = maximum / m;
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
#if 0
	fprintf(outfile, "OFF\n%ld %ld 0\n\n",
		width * height + 2 * (width + height - 2),
		(width - 1) * (height - 1) + 4 * (width + height - 2) + 2);
#endif
	fprintf(outfile, "OFF\n%ld %ld 0\n\n",
		width * height + 2 * width + 2 * (height - 2),
		2 * (width - 1) * (height - 1)
#if XSKIRT
		+ 4 * (width - 1)
#endif /* XSKIRT */
#if YSKIRT
		+ 4 * (height - 1)
#endif /* YSKIRT */
#if BOTTOM
		+ 2
#endif /* BOTTOM */
		);

#define	addpoint(X, Y, Z)						\
	do {								\
		if (debug) { fprintf(stderr, "%s:%d: %d (%f, %f, %f)\n",\
			__FILE__, __LINE__, pointcounter,		\
			X, Y, Z);					\
		}							\
		fprintf(outfile, "%g %g %g\n",				\
			X, Y, Z);					\
		pointcounter++;						\
	} while (0)

	// write all points on individual lines
	long	pointcounter = 0;
	for (long y = 0; y < height; y++) {
		for (long x = 0; x < width; x++) {
			addpoint(x * hx, y * hy,
				scale * data[x + width * y] + offset);
		}
	}
	if (debug) {
		fprintf(stderr, "%s:%d: xaxis points start at: %d\n",
			__FILE__, __LINE__, pointcounter);
	}
	long	xborder = pointcounter;
	for (long x = 0; x < width; x++) {
		addpoint(x * hx, 0., 0.);
		addpoint(x * hx, (height - 1) * hy, 0.);
	}
	if (debug) {
		fprintf(stderr, "%s:%d: yaxis points start at: %d\n",
			__FILE__, __LINE__, pointcounter);
	}
	long	yborder = pointcounter;
	for (long y = 1; y < height - 1; y++) {
		addpoint(0., y * hy, 0.);
		addpoint((width - 1) * hx, y * hy, 0.);
	}
	if (debug) {
		fprintf(stderr, "%s:%d: points: %d\n", __FILE__, __LINE__,
			pointcounter);
	}

	long	trianglecounter = 0;

#define	addquad(a, b, c, d)						\
	do {								\
		if (debug) {						\
			fprintf(stderr, "%s:%d: %d (%ld %ld %ld)\n",	\
				__FILE__, __LINE__,			\
				trianglecounter, a, b, c);		\
		}							\
		fprintf(outfile, "3 %ld %ld %ld\n", a, b, c);		\
		trianglecounter++;					\
		if (debug) {						\
			fprintf(stderr, "%s:%d: %d (%ld %ld %ld)\n",	\
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
	for (long y = 0; y < height - 1; y++) {
		for (long x = 0; x < width - 1; x++) {
			addquad((x    ) + width * (y    ),
				(x + 1) + width * (y    ),
				(x + 1) + width * (y + 1),
				(x    ) + width * (y + 1));
		}
	}

	// write triangles of the boundaries
#if XSKIRT
	if (debug) {
		fprintf(stderr, "%s:%d: add X-skirt triangles\n",
			__FILE__, __LINE__);
	}
	for (int x = 0; x < width - 1; x++) {
		addquad(xborder + 2 * x,
			xborder + 2 * x + 2,
			x + 1,
			x);
		addquad(xborder + 2 * x + 1,
			x + (height - 1) * width,
			x + (height - 1) * width + 1,
			xborder + 2 * x + 3);
	}
#endif /* XSKIRT */
#if YSKIRT
	if (debug) {
		fprintf(stderr, "%s:%d: adding Y-Skirt triangles\n",
			__FILE__, __LINE__);
	}
	for (int y = 1; y < height - 2; y++) {
		addquad(yborder + 2 * (y - 1),
			y * width,
			(y + 1) * width,
			yborder + 2 * y);
		addquad(yborder + 2 * y + 1,
			(y + 1) * width + width - 1,
			y * width + width - 1,
			yborder + 2 * (y - 1) + 1);
	}
	if (debug) {
		fprintf(stderr, "%s:%d: add exceptional quads\n",
			__FILE__, __LINE__);
	}
	addquad(xborder,
		0,
		width,
		yborder);

	addquad(yborder + 1,
		2 * width - 1,
		width - 1,
		xborder + 2 * (width - 1));

	addquad(yborder + 2 * (height - 3),
		(height - 2) * width,
		(height - 1) * width,
		xborder + 1);

	addquad(xborder + 2 * width - 1,
		height * width - 1,
		(height - 1) * width - 1,
		yborder + 2 * (height - 3) + 1);
		
#endif /* YSKIRT */

	// write triangles of the bottom
#if BOTTOM
	if (debug) {
		fprintf(stderr, "%s:%d: add bottom\n", __FILE__, __LINE__);
	}
	addquad(xborder,
		xborder + 1,
		xborder + 2 * width - 1,
		xborder + 2 * width - 2);
#endif
		
	fprintf(outfile, "\n");

	// cleanup
	rc = EXIT_SUCCESS;
	
cleanup:
	// close the output file
	if (outfile) {
		fclose(outfile);
		outfile = NULL;
	}

	if (data) {
		free(data);
		data = NULL;
	}
	if (fits) {
		status = 0;
		fits_close_file(fits, &status);
	}
	free(offname); offname = NULL;

	return rc;
}
