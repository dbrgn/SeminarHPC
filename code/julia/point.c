/*
 * point.c 
 *
 * (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include "point.h"
#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int	parse_point(const char *arg, double *values) {
	int	rc = -1;
	const char	*r = "\\(([-+]?[0-9]+(\\.[0-9]*)?),([-+]?[0-9]+(\\.[0-9]*)?)\\)";
	/* compile regular expression */
	regex_t	regex;
	if (regcomp(&regex, r, 0)) {
		fprintf(stderr, "cannot compile regular expression: %s\n",
			strerror(errno));
		goto end;
	}

	/* try to match */
	regmatch_t	pmatch[3];
	if (0 == regexec(&regex, arg, 3, pmatch, 0)) {
		rc = 0;
		char	*s = (char *)strndup(arg + pmatch[0].rm_so, 
				pmatch[0].rm_eo - pmatch[0].rm_so);
		printf("first match: '%s'\n", s);
		values[0] = atof(s);
		free(s);
		s = (char *)strndup(arg + pmatch[1].rm_so, 
				pmatch[1].rm_eo - pmatch[1].rm_so);
		printf("second match: '%s'\n", s);
		values[1] = atof(s);
		free(s);
	} else {
		fprintf(stderr, "no match with \"%s\"\n", arg);
	}

	/* cleanup */
	regfree(&regex);
end:
	return rc;
}

