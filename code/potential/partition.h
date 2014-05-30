/*
 * partition.h -- partition image into domains
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#ifndef _partition_h
#define _partition_h

#include "domain.h"
#include "image.h"

extern void	partitiondomain(udata_t *u, const image_t *image);
extern void	sendimagerange(const udata_t *u, const image_t *image,
			int rank, int tag);
extern void	receiveimagerange(const udata_t *u, image_t *image, int rank,
			int tag);
extern void	copytoimage(const udata_t *u, image_t *image);
extern void	copyfromimage(udata_t *u, const image_t *image);
extern void	receiverange(udata_t *u, int tag);
extern void	sendrange(const udata_t *u, int tag);
extern void	synchronize_image(const udata_t *u, image_t *image, int tag);

#endif /* _partition_h */
