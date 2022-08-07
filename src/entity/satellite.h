#ifndef __SATELLITE_H__
#define __SATELLITE_H__

#include "TLE.h"
#include "satcat.h"
#include "type.h"

struct Satellite {
	char name[25];
	TLE tle;
	struct SatCat satcat;
	uint32_t orbit_idx;
};

void satellite_init(void);
void satellite_deinit(void);
void satellites_render(void);

void satellites_phys(void);
void satellites_phys_sync(void);

void satellites_get_prep(void);
void *satellites_get_thrd(void *arguments);
void satellites_get_sync(void);

void satellite_select(double xpos, double ypos);
void satellite_select_ptr(struct Satellite *satellite);

#endif /* __SATELLITE_H__ */
