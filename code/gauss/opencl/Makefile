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
	./gauss -P 0 -p 6 -d 10 1000

graphpdfs = vector-amd.pdf vector-intel.pdf vector-nvidia.pdf results.pdf

graphs:	$(graphpdfs)

$(graphpdfs):	results.R results.csv
	R --vanilla --quiet < results.R
	
