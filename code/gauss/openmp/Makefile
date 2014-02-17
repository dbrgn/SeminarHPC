#
# Makefile for gauss implementation, pthreaded
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
#CC = /usr/local/gcc/bin/gcc
CC = gcc
CFLAGS = -std=c99 -o -Wall -O2 -fopenmp -I../common

gauss:	gauss.c
	$(CC) $(CFLAGS) -o gauss gauss.c -L../common -lgauss

test:	gauss
	./gauss 10

results.pdf:	results.csv results.R
	R --vanilla --quiet < results.R
