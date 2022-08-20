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

#ifndef __SATCAT_CODE_LIST_H__
#define __SATCAT_CODE_LIST_H__

#include "satcat_code.h"

#define NUM_SCSTAT 8
#define NUM_SCSRC 117
#define NUM_SCSITE 34

extern const char SCSTAT[NUM_SCSTAT];
extern const sc_code_t SCSRC[NUM_SCSRC];
extern const sc_code_t SCSITE[NUM_SCSITE];

#endif /* __SATCAT_CODE_LIST_H__ */
