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
