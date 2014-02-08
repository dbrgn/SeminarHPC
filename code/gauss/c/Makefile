#
# Makefile -- build basic Gauss algorithm test program
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
gauss:	gauss.c
	gcc -o -Wall -O2 -I ../common -o gauss gauss.c -L../common -lgauss

test:	gauss
	./gauss 10
