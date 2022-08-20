/*
 * Copyright (c) 2022 Wojciech Graj
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 **/

#include "entity.h"

#include "earth.h"
#include "gfx.h"
#include "satellite.h"
#include "sky.h"

void entity_init(void)
{
	earth_init();
#ifndef NO_SATELLITE
	satellite_init();
#endif
	sky_init();
}

void entity_render(void)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);

	earth_render();
#ifndef NO_SATELLITE
	satellites_render();
#endif
	glCullFace(GL_FRONT);
	sky_render();
}

void entity_deinit(void)
{
	earth_deinit();
#ifndef NO_SATELLITE
	satellite_deinit();
#endif
	sky_deinit();
}
