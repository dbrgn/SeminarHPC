#
# convert metapost sources into image to be included in script
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rappersil
# $Id$
# 
all:	work numa laplacian descent \
	cgresults.pdf \
	descent3d.jpg cg3d-center.jpg cg3d-large.jpg

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

descent:	descent-1.pdf descent-2.pdf descent-3.pdf

descent-1.pdf descent-2.pdf descent-3.pdf:	descent.mp
	TEX=latex mpost descent.mp
	mptopdf descent.1
	mptopdf descent.2
	mptopdf descent.3

w = 1920
h = 1080

descent3d.jpg:	descent3d.pov
	povray -Odescent3d.jpg +A -W$(w) -H$(h) descent3d.pov

cgresults.csv:	cgexample.m
	octave < cgexample.m

cgresults.pdf:	cgexample.R cgresults.csv
	R --vanilla --quiet < cgexample.R

cg3d-large.jpg:	cg3d.pov cg3d-large.pov
	povray -Ocg3d-large.jpg -W$(w) -H$(h) +A cg3d-large.pov

cg3d-center.jpg:	cg3d.pov cg3d-center.pov
	povray -Ocg3d-center.jpg -W$(w) -H$(h) +A cg3d-center.pov
