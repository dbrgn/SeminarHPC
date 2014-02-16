#
# Makefile -- build netcdf demo programs
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CFLAGS = -O2 -g -Wall -std=c99
CC = gcc

all:	ncread ncwrite

ncread:	ncread.c
	$(CC) $(CFLAGS) -o ncread ncread.c -lnetcdf -lcfitsio -lm

ncwrite:	ncwrite.c
	$(CC) $(CFLAGS) -o ncwrite ncwrite.c -lnetcdf -lm

test:	all
	rm -f test.nc
	if [ ! -d images ]; then mkdir images; fi
	./ncwrite test.nc
	./ncread test.nc
