#
# Makefile -- build test programs for derotation
#
# (c) 2013 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
CXXFLAGS = -g -Wall -O2
CPPFLAGS = -I/usr/include/opencv2 -I../gauss/common
LDFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgproc -llapack -lfftw3 -lm \
	-lcfitsio -L../gauss/common -lgauss

all:	radontransform filter

radon.o:	radon.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o radon.o radon.cpp

radontransform.o:	radontransform.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o radontransform.o radontransform.cpp

radontransform:	radontransform.o radon.o
	g++ -o radontransform radontransform.o radon.o $(LDFLAGS)

filter:	filter.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -o filter filter.cpp $(LDFLAGS)

filtertest:	filter
	if [ -f out.fits ]; then rm out.fits; fi
	./filter in.fits out.fits

RES=512

test:	radontransform
	./radontransform -s 0.55 -m 0 -w $(RES) -h $(RES) poc.jpg poc-radon.jpg

