#
# Makefile -- common stuff for all test programs
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CFLAGS = -g -Wall -O2 -std=c99

libgauss.a:	common.o
	ar cr libgauss.a common.o

common.o:	common.c common.h

tests:	tests.c libgauss.a
	$(CC) $(CFLAGS) -o tests tests.c -L. -lgauss

clean:
	rm -f common.o libgauss.a
