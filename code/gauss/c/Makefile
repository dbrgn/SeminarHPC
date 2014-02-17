#
# Makefile -- build basic Gauss algorithm test program
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CFLAGS = -g -Wall -O2 -std=c99
CC = gcc

gauss:	gauss.c
	$(CC) $(CFLAGS) -I ../common -o gauss gauss.c -L../common -lgauss

test:	gauss
	./gauss 10

results.pdf results-uni.pdf:	measure.R results.csv results-uni.csv
	R --vanilla --quiet < measure.R
