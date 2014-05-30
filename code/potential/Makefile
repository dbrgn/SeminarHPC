#
# Makefile to build the onedimensional and the MPI version of
# the heat equation solution
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
all:	 potential_mpi

CFLAGS = -Wall -g -O2 -std=c99 -I../gauss/common -I/usr/include/cfitsio
LDFLAGS = -L../gauss/common -lgauss -lnetcdf 

# OpenMPI implementation of 2 dimensional heat equation solver
FILES2 = output.c image.c domain.c iteration.c partition.c boundary.c \
	potential_mpi.c

potential_mpi:	$(FILES2)
	mpicc $(CFLAGS) -o potential_mpi $(FILES2) $(LDFLAGS) -lcfitsio

testmpi:	potential_mpi
	rm -f out.nc
	rm -f images/*.fits
	time mpirun -n 4 ./potential_mpi -b images -h 0.1 -x 2 -y 2 -n 128 -a 2\
		testimage_128_3.fits 2>&1 | tee test.log

