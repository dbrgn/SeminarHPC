#
# convert metapost sources into image to be included in script
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rappersil
# $Id$
# 
all:	work

work:	work-1.pdf

work-1.pdf:	work.mp
	TEX=latex mpost work.mp
	mptopdf work.1
