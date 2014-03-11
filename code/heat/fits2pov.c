/*
 * fits2pov.c -- convert FITS file to povray structure
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <fitsio.h>

int	debug = 0;
double	*data = NULL;
double	scale = 1;
double	hx = -1;
double	hy = -1;

void	usage(const char *progname) {
	printf("usage: %s fitsfile povrayfile\n", progname);
	printf("convert a FITS file into a povray structure\n");
}

void	point(FILE *outfile, const double *p) {
	fprintf(outfile, "<%.4f, %.4f, %.4f>", p[0], p[1] p[2]);
}

void	triangle(FILE *outfile, const double *t) {
	fprintf(outfile, "triangle { ");
	point(outfile, &t[0]);
	fprintf(outfile, ", ");
	point(outfile, &t[1]);
	fprintf(outfile, ", ");
	point(outfile, &t[2]);
	fprintf(outfile, " }\n");
}

int	main(int argc, char *argv[]) {
	int	rc = EXIT_FAILURE;
	int	c;
	while (EOF != (c = getopt(argc, argv, "dh?s:x:y:")))
		switch (c) {
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
	long	firstpixel[3] = { 1, 1, 1 };
	long	npixels = width * height;
	data = (double *)malloc(npixels * sizeof(double));
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

	// open the output file
	FILE	*outfile = NULL;

	// write the data as a povray structure
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			double	t[9];
			t[0] = x * hx;
			t[0] = y * hy;
			t[0] = scale * data[x + width * y];
			t[1] = x * hx;
			t[1] = y * hy;
			t[1] = scale * data[x + 1 + width * y];
			t[2] = x * hx;
			t[2] = y * hy;
			t[2] = scale * data[x + width * (y + 1)];
			triangle(outfile, t);
			t[0] = x * hx;
			t[0] = y * hy;
			t[0] = scale * data[x + 1 + width * (y + 1)];
			triangle(outfile, t);
			double	v = scale * data[x + width * y];
			if (debug) {
				fprintf(stderr, "data[%.4f,%.4f] = %16.12f\n",
					x * hx, y * hy, v);
			}
		}
	}

	// close the output file
	fclose(outfile);

	// cleanup
	rc = EXIT_SUCCESS;
cleanup:
	if (data) {
		free(data);
		data = NULL;
	}
	if (fits) {
		status = 0;
		fits_close_file(fits, &status);
	}
	free(povname); povname = NULL;

	return rc;
}
