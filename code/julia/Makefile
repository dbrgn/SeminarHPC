#
# Makefile -- build example programs for the julia set computation
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CC = gcc
CFLAGS = -std=c99 -g -O2 -Wall -I../gauss/common -I/opt/AMDAPP/include -I/usr/include/cfitsio 
LDFLAGS = -L../gauss/common -lgauss -L/opt/AMDAPP/lib/x86_64 -lOpenCL -lcfitsio -lm

all:	julia1 julia2

julia1:	julia1.c point.c point.h color.c color.h mono.c mono.h
	$(CC) $(CFLAGS) -o julia1 julia1.c point.c color.c mono.c $(LDFLAGS)
		

julia2:	julia2.c point.c point.h
	$(CC) $(CFLAGS) -o julia2 julia2.c point.c $(LDFLAGS)

test:	test1 test2

test1:	julia1
	time ./julia1 -P 0 -d -e -c '(0,0.66)' out1.fits

test2:	julia2
	time ./julia2 -P 0 -d -s -i 1000 -n 10000 out2.fits
