#
# Makefile to build the onedimensional and the MPI version of
# the heat equation solution
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
all:	heat heat_mpi

CFLAGS = -Wall -g -O2 -std=c99 -I../gauss/common -I/usr/include/cfitsio
LDFLAGS = -L../gauss/common -lgauss -lnetcdf 

# OpenMP implementation of 1 dimension heat equation solver
FILES = output.c heat.c

heat:	$(FILES)
	$(CC) $(CFLAGS) -fopenmp -o heat $(FILES) $(LDFLAGS)

test:	heat
	rm -f out.nc
	./heat -n 999 -s 1 -t 0.1 out.nc

# OpenMPI implementation of 2 dimensional heat equation solver
FILES2 = output.c image.c domain.c iteration.c partition.c boundary.c \
	heat_mpi.c

heat_mpi:	$(FILES2)
	mpicc $(CFLAGS) -o heat_mpi $(FILES2) $(LDFLAGS) -lcfitsio

testmpi:	heat_mpi
	rm -f out.nc
	rm -f images/*.fits
	time mpirun -n 4 ./heat_mpi -b images -t 4 -s 2 -h 0.1 -x 2 -y 2 \
		testimage.fits 2>&1 | tee test.log

# measurements
results-heat.pdf:	results.csv results.R
	R --vanilla --quiet < results.R
