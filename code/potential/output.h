/*
 * output.h -- output functions for heat equation simulation
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _OUTPUT_H
#define _OUTPUT_H

typedef struct {
	int	ncid;
	int	arrayid;
	int	dim;
	int	nx;
	int	ny;
} heatfile_t;

extern heatfile_t	*output_create(const char *filename,
				double hx, double ht, int n);
extern int	output_add(heatfile_t *hf, int t, double *u);

extern heatfile_t	*output2_create(const char *filename,
				double h, double ht, int nx, int ny);
extern int	output2_add(heatfile_t *hf, int t, double *u);

extern int	output_close(heatfile_t *hf);

#endif /* _OUTPUT_H */
