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
