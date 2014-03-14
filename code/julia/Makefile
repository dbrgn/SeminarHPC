#
# Makefile -- build example programs for the julia set computation
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CC = gcc
CFLAGS = -std=c99 -g -O2 -Wall -I../gauss/common -I/opt/AMDAPP/include -I/usr/include/cfitsio 
LDFLAGS = -L../gauss/common -lgauss -L/opt/AMDAPP/lib/x86_64 -lOpenCL -lcfitsio

all:	julia

julia:	julia.c
	$(CC) $(CFLAGS) -o julia julia.c $(LDFLAGS)
		

julia2:	julia2.c
	$(CC) $(CFLAGS) -o julia2 julia2.c $(LDFLAGS)

test:	julia
	./julia -P 0 -d out.fits

test2:	julia2
	./julia2 -P 0 -d -N 100 -i 1000 -n 1000000 out2.fits
