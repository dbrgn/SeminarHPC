#
# Makefile -- build the gauss example
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CC = gcc
CFLAGS = -g -O2 -Wall -I../common

gauss:	gauss.c
	$(CC) $(CFLAGS) -o gauss gauss.c -framework opencl -L../common -lgauss

test:	gauss
	./gauss
