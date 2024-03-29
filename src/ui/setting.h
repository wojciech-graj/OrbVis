/*
 * Copyright (c) 2022-2023 Wojciech Graj
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

#ifndef __SETTING_H__
#define __SETTING_H__

#include "type.h"
#include "ui.h"

enum ReferenceFrame {
	REFERENCE_FRAME_FIXED = 0u,
	REFERENCE_FRAME_INERTIAL,
};

extern gboolean es_localtime;
extern gboolean es_clouds;
extern gboolean es_lighting;
extern gboolean es_invert_scroll;
extern guint es_satellite_scale;
extern enum ReferenceFrame es_reference_frame;

void setting_init(GtkBuilder *builder);
void setting_deinit(void);

#endif /* __SETTING_H__ */
