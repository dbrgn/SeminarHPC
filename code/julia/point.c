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

extern int	debug;

int	parse_point(const char *arg, double *values) {
	int	rc = -1;
	const char	*r
		= "(\\([+-]\\?[0-9]*\\(\\.[0-9]*\\)\\?\\),\\([+-]\\?[0-9]*\\(\\.[0-9]*\\)\\?\\))";
	if (debug) {
		fprintf(stderr, "%s:%d: regular expression: %s\n",
			__FILE__, __LINE__, r);
	}
	/* compile regular expression */
	regex_t	regex;
	if (regcomp(&regex, r, 0)) {
		fprintf(stderr, "cannot compile regular expression: %s\n",
			strerror(errno));
		goto end;
	}

	/* try to match */
#define	nmatches 5
	regmatch_t	pmatch[nmatches];
	if (0 == regexec(&regex, arg, 5, pmatch, 0)) {
		rc = 0;
		for (int i = 0; i < nmatches; i++) {
			char	*s = strndup(arg + pmatch[i].rm_so, 
					pmatch[i].rm_eo - pmatch[i].rm_so);
			if (debug) {
				fprintf(stderr, "%s:%d: match[%d]: '%s'\n",
					__FILE__, __LINE__, i, s);
			}
			switch (i) {
			case 1:	values[0] = atof(s);
				break;
			case 3:	values[1] = atof(s);
				break;
			default:
				break;
			}
			free(s);
		}
	} else {
		fprintf(stderr, "no match with \"%s\"\n", arg);
	}

	/* cleanup */
	regfree(&regex);
end:
	if ((debug) && (rc == 0)) {
		fprintf(stderr, "%s:%d: point: %.6f + %.6fi\n",
			__FILE__, __LINE__, values[0], values[1]);
	}
	return rc;
}

int	parse_cpoint(const char *argc, double complex *v) {
	double	values[2];
	if (parse_point(argc, values) < 0) {
		return -1;
	}
	*v = values[0] + values[1] * I;
	return 0;
}
