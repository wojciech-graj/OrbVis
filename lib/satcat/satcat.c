/*
 * Copyright (c) 2022-2024 Wojciech Graj
 *
 * Licensed under the MIT license: https://opensource.org/licenses/MIT
 * Permission is granted to use, copy, modify, and redistribute the work.
 * Full license information available in the project LICENSE file.
 **/

#include "satcat.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static scbool isopstat(char c);
static int parse_int(const char *str, unsigned len);
static double parse_dbl(const char *str, unsigned len, unsigned loc_pt);
static void parse_date(const char *str, struct SCDate *date);

void sc_parse(struct SatCat *sc, const char *str)
{
	memcpy(sc->id, str, 11);
#ifdef SC_CSTRING
	sc->id[11] = '\0';
#endif
	sc->catnum = parse_int(str + 13, 5);
	sc->mul_names = (str[19] == 'M');
	sc->payload = (str[20] == '*');
	sc->opstat = str[21];
	memcpy(sc->name, str + 23, 24);
#ifdef SC_CSTRING
	sc->name[24] = '\0';
#endif
	memcpy(sc->source, str + 49, 5);
#ifdef SC_CSTRING
	sc->source[5] = '\0';
#endif
	parse_date(str + 56, &sc->launch_date);
	memcpy(sc->launch_site, str + 68, 5);
	if (str[75] != ' ') {
		parse_date(str + 75, &sc->decay_date);
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
#ifdef SC_CSTRING
	sc->status_code[3] = '\0';
#endif
}

scbool sc_validate(const char *str)
{
	const char *fmt = "nnnn-nnnaaa  nnnnn M*O aaaaaaaaaaaaaaaaaaaaaaaa  aaaaa  nnnn-nn-nn  aaaaa  lnnn-nn-nn  NNNNn.n  NNn.n  NNNNNn  NNNNNn  LNn.nnnn  aaa";
	unsigned i;

	for (i = 0; i < 132; i++) {
		/* Validate Length */
		if (str[i] == '\0')
			return 0;

		/* Validate Contents */
		switch (fmt[i]) {
		case 'l':
			if (!strncmp("          ", str + 75, 10)) {
				i += 9;
				continue;
			}
			__attribute__((fallthrough));
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
		case 'L':
			if (!strncmp("   N/A  ", str + 119, 8)) {
				i += 7;
				continue;
			}
			__attribute__((fallthrough));
		case 'N':
			if ((fmt[i - 1] == 'N' && str[i - 1] != ' ' && str[i] == ' ')
				|| (str[i] != ' ' && !isdigit(str[i])))
				return 0;
			break;
		default:
			if (str[i] != fmt[i])
				return 0;
		}
	}
	return 1;
}

scbool isopstat(const char c)
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

int parse_int(const char *str, const unsigned len)
{
	char buf[8];
	memcpy(buf, str, len);
	buf[len] = '\0';
	return strtol(buf, NULL, 10);
}

double parse_dbl(const char *str, const unsigned len, const unsigned loc_pt)
{
	const unsigned decim_places = len - loc_pt - 1;
	const int integral = parse_int(str, loc_pt);
	const int fract = parse_int(str + loc_pt + 1, decim_places);
	return integral + fract / pow(10.0, decim_places);
}

void parse_date(const char *str, struct SCDate *date)
{
	date->year = parse_int(str, 4);
	date->month = parse_int(str + 5, 2);
	date->day = parse_int(str + 8, 2);
}
