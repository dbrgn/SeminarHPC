/*
 * mono.h -- write a mono image
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswi
 */
#ifndef _mono_h
#define _mono_h

extern int	write_mono(const char *filename, const int width,
			const int height, const unsigned short *pixels);

#endif /* _mono_h */
