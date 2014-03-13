#
# Makefile -- build example programs for the julia set computation
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CC = gcc
CFLAGS = -std=c99 -g -O2 -Wall -I../gauss/common -I/opt/AMDAPP/include

julia:	julia.c
	$(CC) $(CFLAGS) -o julia julia.c -L../gauss/common -lgauss \
		-L/opt/AMDAPP/lib/x86_64 -lOpenCL

test:	julia
	./julia -P 0 -d -n 1000
