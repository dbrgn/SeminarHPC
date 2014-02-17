#
# Makefile for gauss implementation, pthreaded
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CC = gcc
CFLAGS = -Wall -O2 -g -std=c99 -I../common 

gauss:	gauss.c
	$(CC) $(CFLAGS) -o gauss gauss.c -L../common -lgauss -lpthread

test:	gauss
	./gauss 10

results.pdf:	results.csv
	R --vanilla --quiet < results.R
