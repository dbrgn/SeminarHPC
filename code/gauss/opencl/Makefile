#
# Makefile -- build the gauss example
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CC = gcc
CFLAGS = -std=c99 -g -O2 -Wall -I../common -I/opt/AMDAPP/include

gauss:	gauss.c
	$(CC) $(CFLAGS) -o gauss gauss.c -L../common -lgauss -L/opt/AMDAPP/lib/x86_64 -lOpenCL

test:	gauss
	./gauss -p 6 -d -D -n 10
