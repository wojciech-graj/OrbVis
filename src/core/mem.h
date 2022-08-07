#ifndef __MEM_H__
#define __MEM_H__

#include <stdlib.h>

void *safe_malloc(size_t size);
void *safe_realloc(void *ptr, size_t size);
void *safe_calloc(size_t nmemb, size_t size);

#endif /* __MEM_H__ */
