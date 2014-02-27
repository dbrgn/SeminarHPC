#
# Makefile to build the MPI version of the heat equation solution
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CFLAGS = -Wall -g -O2 -std=c99 -I../gauss/common
LDFLAGS = -L../gauss/common -lgauss -lnetcdf

# OpenMP implementation of 1 dimension heat equation solver
FILES = output.c heat.c

heat:	$(FILES)
	$(CC) $(CFLAGS) -fopenmp -o heat $(FILES) $(LDFLAGS)

test:	heat
	rm -f out.nc
	./heat -n 999 -s 1 -t 0.1 out.nc

# measurements
results.pdf:	results.csv results.R
	R --vanilla --quiet < results.R
