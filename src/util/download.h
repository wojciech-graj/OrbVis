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

#ifndef __DOWNLOAD_H__
#define __DOWNLOAD_H__

#include "type.h"

#include <curl/curl.h>

struct DLHandle {
	CURL *curl;
	guint8 *memory;
	size_t size;
};

struct DLMulti {
	unsigned n_ops;
	CURLM *multi_handle;
	struct DLHandle *handles;
};

void dl_init(void);
void dl_multi_init(struct DLMulti *dl_multi, unsigned n_ops, const char **urls);
void dl_multi_deinit(struct DLMulti *dl_multi);
void dl_multi_perform(struct DLMulti *dl_multi);

#endif /* __DOWNLOAD_H__ */
