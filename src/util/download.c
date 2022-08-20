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

#include "error.h"

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp);

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct DLHandle *mem = (struct DLHandle *)userp;

	guint8 *ptr = g_realloc(mem->memory, mem->size + realsize + 1);

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

void dl_init(void)
{
	CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
	error_check(!code, "Failed to initialize curl.");
}

void dl_multi_init(struct DLMulti *dl_multi, unsigned n_ops, const char **urls)
{
	dl_multi->n_ops = n_ops;
	dl_multi->multi_handle = curl_multi_init();
	error_check(dl_multi->multi_handle, "Failed to initialize curl multi handle.");
	dl_multi->handles = g_malloc(sizeof(struct DLHandle) * n_ops);
	unsigned i;
	for (i = 0; i < n_ops; i++) {
		struct DLHandle *handle = &dl_multi->handles[i];
		handle->curl = curl_easy_init();
		error_check(handle->curl, "Failed to initialize curl handle.");
		curl_easy_setopt(handle->curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(handle->curl, CURLOPT_WRITEDATA, (void *)handle);
		curl_easy_setopt(handle->curl, CURLOPT_URL, urls[i]);
		curl_multi_add_handle(dl_multi->multi_handle, handle->curl);

		handle->memory = NULL;
	}
}

void dl_multi_perform(struct DLMulti *dl_multi)
{
	/* Prepare Memory */
	unsigned i;
	for (i = 0; i < dl_multi->n_ops; i++) {
		struct DLHandle *handle = &dl_multi->handles[i];
		handle->memory = g_malloc(1);
		handle->size = 0;
	}

	/* Download */
	int running_handles;
	do {
		CURLMcode mc = curl_multi_perform(dl_multi->multi_handle, &running_handles);
		if (!mc && running_handles)
			mc = curl_multi_poll(dl_multi->multi_handle, NULL, 0, 1000, NULL);

		error_check(!mc, "Failed to curl_multi_poll. Error code %d.", mc);
	} while (running_handles);
}

void dl_multi_deinit(struct DLMulti *dl_multi)
{
	unsigned i;
	for (i = 0; i < dl_multi->n_ops; i++) {
		struct DLHandle *handle = &dl_multi->handles[i];
		curl_multi_remove_handle(dl_multi->multi_handle, handle->curl);
		curl_easy_cleanup(handle->curl);
		g_free(handle->memory);
	}
	curl_multi_cleanup(dl_multi->multi_handle);
}
