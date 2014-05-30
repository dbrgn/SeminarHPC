/*
 * utils.h -- some common functions
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _utils_h
#define _utils_h

typedef struct image_s {
	long	width;
	long	height;
	double	*data;
} image_t;

extern image_t	*readfits(const char *fitsname);
extern long	npixels(const image_t *image);
extern double	datamax(const image_t *image);
extern void	freeimage(image_t *image);
extern double	value(const image_t *image, long x, long y);

#endif /* _utils_h */
