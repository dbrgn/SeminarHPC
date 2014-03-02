/*
 * radontransform.cpp -- perform the radon transform of an image
 *
 * (c) 2013 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdio.h>
#include <stdlib.h>
#include <opencv.hpp>
#include "radon.h"
#include <iostream>
#include <getopt.h>

int	main(int argc, char *argv[]) {
	int	c;
	int	width = 512;
	int	height = 512;
	double	scale = 1;
	bool	mask = false;

	while (EOF != (c = getopt(argc, argv, "w:h:m:M:s:")))
		switch (c) {
		case 'w':
			width = atoi(optarg);
			break;
		case 'h':
			height = atoi(optarg);
			break;
		case 'm':
			margin = atof(optarg);
			break;
		case 'M':
			mask = true;
			break;
		case 's':
			scale = atof(optarg);
			break;
		}	

	if ((argc - optind) != 2) {
		std::cerr << "need exactly two file name arguments" << std::endl;
		exit(EXIT_FAILURE);
	}

	const char	*infile = argv[optind++];
	const char	*outfile = argv[optind];

	// read the image
	cv::Mat	r = radon(infile, width, height, scale, mask);

	// write the result
	imwrite(std::string(outfile), r);
}
