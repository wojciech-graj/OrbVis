#ifndef __SETTING_H__
#define __SETTING_H__

#include "type.h"
#include "ui.h"

enum ReferenceFrame {
	REFERENCE_FRAME_FIXED = 0u,
	REFERENCE_FRAME_INERTIAL,
};

extern gboolean gs_gmt;
extern gboolean gs_clouds;
extern gboolean gs_lighting;
extern enum ReferenceFrame gs_reference_frame;

void setting_init(GtkBuilder *builder);

#endif /* __SETTING_H__ */
