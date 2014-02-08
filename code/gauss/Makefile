#
# Build all example programs
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
all:
	for d in common c pthread openmp opencl openmpi; \
	do \
		cd $${d}; make; cd ..; \
	done
