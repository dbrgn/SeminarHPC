/*
 * output.c -- create netcdf file for heat equation simulation
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include "output.h"
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * \brief create the output file
 */
heatfile_t	*output_create(const char *filename, double hx, double ht,
	int n) {
	heatfile_t	*hf = (heatfile_t *)malloc(sizeof(heatfile_t));
	hf->n = n;
	hf->ncid = -1;

	int	status;

	// create the file
	status = nc_create(filename, NC_NOCLOBBER, &hf->ncid);
	if (NC_NOERR != status) {
		fprintf(stderr, "cannot create file %s: %s\n", filename,
			nc_strerror(status));
	}

	// add variables hx, hy, and n
	int	hxid, htid, nid;
	status = nc_def_var(hf->ncid, "hx", NC_DOUBLE, 0, NULL, &hxid);
	if (NC_NOERR != status) {
		fprintf(stderr, "cannot create hx variable: %s\n",
			nc_strerror(status));
		goto bad;
	}
	status = nc_def_var(hf->ncid, "ht", NC_DOUBLE, 0, NULL, &htid);
	if (NC_NOERR != status) {
		fprintf(stderr, "cannot create ht variable: %s\n",
			nc_strerror(status));
		goto bad;
	}
	status = nc_def_var(hf->ncid, "n", NC_INT, 0, NULL, &nid);
	if (NC_NOERR != status) {
		fprintf(stderr, "cannot create n variable: %s\n",
			nc_strerror(status));
		goto bad;
	}

	// create the array dimensions
	size_t	lenx = n;
	int	x_dim, t_dim;
	if (NC_NOERR != (status = nc_def_dim(hf->ncid, "x", lenx, &x_dim))) {
		fprintf(stderr, "cannot define x dimension: %s\n",
			nc_strerror(status));
		goto bad;
	}
	if (NC_NOERR != (status = nc_def_dim(hf->ncid, "t", NC_UNLIMITED,
		&t_dim))) {
		fprintf(stderr, "cannot define t dimension: %s\n",
			nc_strerror(status));
		goto bad;
	}

	// define the array
	int	dimensions[2] = { t_dim, x_dim };
	if (NC_NOERR != (status = nc_def_var(hf->ncid, "u", NC_DOUBLE,
		2, dimensions, &hf->arrayid))) {
		fprintf(stderr, "cannot define u array: %s\n",
			nc_strerror(status));
		goto bad;
	}

	// end define mode
	if (NC_NOERR != (status = nc_enddef(hf->ncid))) {
		fprintf(stderr, "cannot end define mode: %s\n",
			nc_strerror(status));
		goto bad;
	}

	// add values for hx, ht and n
	status = nc_put_var_double(hf->ncid, hxid, &hx);
	if (NC_NOERR != status) {
		fprintf(stderr, "cannot write the hx value: %s\n",
			nc_strerror(status));
		goto bad;
	}
	status = nc_put_var_double(hf->ncid, htid, &ht);
	if (NC_NOERR != status) {
		fprintf(stderr, "cannot write the ht value: %s\n",
			nc_strerror(status));
		goto bad;
	}
	status = nc_put_var_int(hf->ncid, nid, &n);
	if (NC_NOERR != status) {
		fprintf(stderr, "cannot write the n value: %s\n",
			nc_strerror(status));
		goto bad;
	}

	return hf;
bad:
	if (hf->ncid >= 0) {
		nc_close(hf->ncid);
	}
	free(hf);
	return NULL;
}

/**
 * \brief add a row to the result fiel
 */
int	output_add(heatfile_t *hf, int t, double *u) {
	size_t	start[2] = { t, 0 };
	size_t	size[2] = { 1, hf->n };
	int	status = nc_put_vara(hf->ncid, hf->arrayid, start, size, u);
	if (NC_NOERR != status) {
		fprintf(stderr, "cannot write data: %s\n",
			nc_strerror(status));
	}
	return 0;
}

/**
 * \brief close the output file
 */
int	output_close(heatfile_t *hf) {
	int	status;
	if (NC_NOERR != (status = nc_close(hf->ncid))) {
		fprintf(stderr, "cannot close file: %s\n", nc_strerror(status));
		return -1;
	}
	free(hf);
	return 0;
}
