/*
 * image.h -- image read/write routines
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _image_h
#define _image_h

typedef struct {
	int	width;
	int	height;
	double	*data;
} image_t;

extern image_t	*readimage(const char *filename);
extern void	writeimage(const image_t *image, const char *filename);

#endif /* _image_h */
