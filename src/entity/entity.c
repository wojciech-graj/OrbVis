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
