#include "mem.h"

#include "error.h"

void *safe_malloc(const size_t size)
{
	void *ptr = malloc(size);
	error_check(ptr, "Unable to allocate [%zu] bytes on heap.", size);
	return ptr;
}

void *safe_realloc(void *ptr, const size_t size)
{
	void *new_ptr = realloc(ptr, size);
	error_check(new_ptr, "Unable to reallocate [%zu] bytes on heap.", size);
	return new_ptr;
}

void *safe_calloc(const size_t nmemb, const size_t size)
{
	void *ptr = calloc(nmemb, size);
	error_check(ptr, "Unable to calloc [%zu] bytes on the heap.", nmemb * size);
	return ptr;
}
