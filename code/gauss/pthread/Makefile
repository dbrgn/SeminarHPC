#
# Makefile for gauss implementation, pthreaded
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CC = gcc
CFLAGS = -Wall -O2 -g -I../common

gauss:	gauss.c
	$(CC) $(CFLAGS) -o gauss gauss.c -L../common -lgauss

test:	gauss
	./gauss 10
