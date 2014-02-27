#
# Makefile to build the MPI version of the Gauss algorithm
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CC = mpicc
CFLAGS = -Wall -g -O2 -std=c99 -I../gauss/common

heat:	heat.c
	$(CC) $(CFLAGS) -o heat heat.c -L../gauss/common -lgauss

test:	heat
	mpirun -np 2 ./heat

results.pdf threads.pdf:	results.csv results.R
	R --vanilla --quiet < results.R
