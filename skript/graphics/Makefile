#
# convert metapost sources into image to be included in script
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rappersil
# $Id$
# 
all:	work numa laplacian descent

work:	work-1.pdf

work-1.pdf:	work.mp
	TEX=latex mpost work.mp
	mptopdf work.1

numa:	numa-1.pdf

numa-1.pdf:	numa.mp
	TEX=latex mpost numa.mp
	mptopdf numa.1

laplacian:	laplacian-1.pdf

laplacian-1.pdf:	laplacian.mp
	TEX=latex mpost laplacian.mp
	mptopdf laplacian.1

descent:	descent-1.pdf

descent-1.pdf:	descent.mp
	TEX=latex mpost descent.mp
	mptopdf descent.1
