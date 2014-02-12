#
# Makefile -- build netcdf demo programs
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CFLAGS = -O2 -g -Wall -std=c99
CC = gcc

all:	ncread ncwrite

ncread:	ncread.c
	$(CC) $(CFLAGS) -o ncread ncread.c -lnetcdf -lcfitsio

ncwrite:	ncwrite.c
	$(CC) $(CFLAGS) -o ncwrite ncwrite.c -lnetcdf 

test:	all
	rm -f test.nc
	./ncwrite test.nc
	./ncread test.nc
