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
#else
typedef int bool;
#endif

struct SCDate {
	unsigned year;
	unsigned month;
	unsigned day;
};

struct SatCat {
	char id[12]; /* International Designator */ /* NOTE: NULL-Terminated */
	unsigned catnum; /* NORAD Catalog Number */
	bool mul_names; /* Multiple Name Flag */
	bool payload; /* Payload Flag */
	char opstat; /* Operational Status Code */
	char name[24]; /* Satellite Name */
	char source[5]; /* Source or Ownership */
	struct SCDate launch_date; /* Launch Date */
	char launch_site[5]; /* Launch Site */
	struct SCDate decay_date; /* Decay Date [Optional] */
	double period; /* Orbital Period (minutes) */
	double inc_deg; /* Inclination (degrees) */
	unsigned apogee; /* Apogee Altitude (kilometers) */
	unsigned perigee; /* Perigee Altitude (kilometers) */
	double radar_cs; /* Radar Cross Section (meters^2) [Optional] */
	char status_code[3]; /* Orbital Status Code */
};

void sc_parse(struct SatCat *sc, char *str);
bool sc_validate(char *str);

#endif /* SATCAT_H */
