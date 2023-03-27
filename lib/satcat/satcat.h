/*
 * Copyright (c) 2022 Wojciech Graj
 *
 * Licensed under the MIT license: https://opensource.org/licenses/MIT
 * Permission is granted to use, copy, modify, and redistribute the work.
 * Full license information available in the project LICENSE file.
 **/

#ifndef SATCAT_H
#define SATCAT_H

#ifdef __STDC_VERSION__
#include <stdbool.h>
typedef bool scbool;
#else
typedef int scbool;
#endif

#ifdef SC_CSTRING
#define SC_TERM 1
#else
#define SC_TERM 0
#endif

struct SCDate {
	unsigned year;
	unsigned month;
	unsigned day;
};

struct SatCat {
	char id[11 + SC_TERM]; /* International Designator */
	unsigned catnum; /* NORAD Catalog Number */
	scbool mul_names; /* Multiple Name Flag */
	scbool payload; /* Payload Flag */
	char opstat; /* Operational Status Code */
	char name[24 + SC_TERM]; /* Satellite Name */
	char source[5 + SC_TERM]; /* Source or Ownership */
	struct SCDate launch_date; /* Launch Date */
	char launch_site[5 + SC_TERM]; /* Launch Site */
	struct SCDate decay_date; /* Decay Date [Optional] */
	double period; /* Orbital Period (minutes) */
	double inc_deg; /* Inclination (degrees) */
	unsigned apogee; /* Apogee Altitude (kilometers) */
	unsigned perigee; /* Perigee Altitude (kilometers) */
	double radar_cs; /* Radar Cross Section (meters^2) [Optional] */
	char status_code[3 + SC_TERM]; /* Orbital Status Code */
};

void sc_parse(struct SatCat *sc, const char *str);
scbool sc_validate(const char *str);

#endif /* SATCAT_H */
