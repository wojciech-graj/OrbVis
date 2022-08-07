/*
 * Copyright (c) 2022 Wojciech Graj
 *
 * Licensed under the MIT license: https://opensource.org/licenses/MIT
 * Permission is granted to use, copy, modify, and redistribute the work.
 * Full license information available in the project LICENSE file.
 **/

#include "satcat.h"

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

static bool isopstat(char c);
static int parse_int(char *str, unsigned len);
static double parse_dbl(char *str, unsigned len, unsigned loc_pt);
static void parse_date(struct SCDate *date, char *str);

void sc_parse(struct SatCat *sc, char *str)
{
	memcpy(sc->id, str, 11);
	sc->id[11] = '\0';
	sc->catnum = parse_int(str + 13, 5);
	sc->mul_names = (str[19] == 'M');
	sc->payload = (str[20] == '*');
	sc->opstat = str[21];
	memcpy(sc->name, str + 23, 24);
	memcpy(sc->source, str + 49, 5);
	parse_date(&sc->launch_date, str + 56);
	memcpy(sc->launch_site, str + 68, 5);
	if (str[75] != ' ') {
		parse_date(&sc->decay_date, str + 75);
	} else {
		sc->decay_date.year = 0;
		sc->decay_date.month = 0;
		sc->decay_date.day = 0;
	}
	sc->period = parse_dbl(str + 87, 7, 5);
	sc->inc_deg = parse_dbl(str + 96, 5, 3);
	sc->apogee = parse_int(str + 103, 6);
	sc->perigee = parse_int(str + 111, 6);
	if (str[122] == '.')
		sc->radar_cs = parse_dbl(str + 119, 8, 3);
	else
		sc->radar_cs = 0.0;
	memcpy(sc->status_code, str + 129, 3);
}

bool sc_validate(char *str)
{
	const char *fmt = "nnnn-nnnaaa  nnnnn M*O aaaaaaaaaaaaaaaaaaaaaaaa  aaaaa  nnnn-nn-nn  aaaaa  nnnn-nn-nn  NNNNn.n  NNn.n  NNNNNn  NNNNNn  NNn.nnnn  aaa";
	unsigned i;

	/* Validate Length */
	for (i = 0; i < 132; i++) {
		if (str[i] == '\0')
			return 0;
	}

	/* Validate Contents */
	for (i = 0; i < 132; i++) {
		if (i == 75) {
			if (!strncmp("          ", str + 75, 10)) {
				i += 9;
				continue;
			}
		} else if (i == 119) {
			if (!strncmp("   N/A  ", str + 119, 8)) {
				i += 7;
				continue;
			}
		}

		switch (fmt[i]) {
		case 'n':
			if (!isdigit(str[i]))
				return 0;
			break;
		case 'a':
			if (!isgraph(str[i]) && str[i] != ' ')
				return 0;
			break;
		case 'M':
		case '*':
			if (str[i] != ' ' && str[i] != fmt[i])
				return 0;
			break;
		case 'O':
			if (!isopstat(str[i]))
				return 0;
			break;
		case 'N':
			if ((fmt[i - 1] == 'N' && str[i - 1] != ' ' && str[i] == ' ') || (str[i] != ' ' && !isdigit(str[i])))
				return 0;
			break;
		default:
			if (str[i] != fmt[i])
				return 0;
		}
	}
	return 1;
}

bool isopstat(char c)
{
	switch (c) {
	case '+':
	case '-':
	case 'P':
	case 'B':
	case 'S':
	case 'X':
	case 'D':
	case '?':
	case ' ':
		return 1;
	default:
		return 0;
	}
}

int parse_int(char *str, unsigned len)
{
	char buf[8];
	memcpy(buf, str, len);
	buf[len] = '\0';
	return strtol(buf, NULL, 10);
}

double parse_dbl(char *str, unsigned len, unsigned loc_pt)
{
	unsigned decim_places = len - loc_pt - 1;
	int integral = parse_int(str, loc_pt);
	int fract = parse_int(str + loc_pt + 1, decim_places);
	return integral + fract / pow(10.0, decim_places);
}

void parse_date(struct SCDate *date, char *str)
{
	date->year = parse_int(str, 4);
	date->month = parse_int(str + 5, 2);
	date->day = parse_int(str + 8, 2);
}
