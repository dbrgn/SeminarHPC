#
# Makefile to build the MPI version of the Gauss algorithm
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CC = mpicc
CFLAGS = -Wall -g -O2 -std=c99 -I../common

gauss:	gauss.c
	$(CC) $(CFLAGS) -o gauss gauss.c -L../common -lgauss

test:	gauss
	mpirun -np 2 ./gauss

results.pdf:	results.csv results.R
	R --vanilla --quiet < results.R
