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

#ifndef __VERSION_H__
#define __VERSION_H__

#include "type.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 1

#ifdef DEBUG
#define DEBUG_STRING "-debug"
#else
#define DEBUG_STRING
#endif

#define VERSION_STRING      \
	XSTR(VERSION_MAJOR) \
	"." XSTR(VERSION_MINOR) "." XSTR(VERSION_PATCH) DEBUG_STRING

#define ABOUT_STRING "OrbVis " VERSION_STRING "\nCopyright (c) 2022 Wojciech Graj" \
		     "\nLicense GPLv2+: GNU GPL version 2 or later."

#endif /* __VERSION_H__ */
