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

#include "download.h"
#include "entity.h"
#include "phys.h"
#include "render.h"
#include "system.h"
#include "thread.h"
#include "ui.h"

int main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	system_init();
	dl_init();
	ui_init(argc, &argv);
	render_init();
	phys_init();
	entity_init();

#ifndef NO_SATELLITE
	thread_dispatch(THRD_SATELLITES_GET, NULL);
#endif

	gtk_main();

	return 0;
}
