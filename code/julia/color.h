/*
 * color.h -- write an array as a color image
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _color_h
#define _color_h

extern int	write_color(const char *filename, const int width,
			const int height, const unsigned short *pixels);

#endif /* _color_h */
