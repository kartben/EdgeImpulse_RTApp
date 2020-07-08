#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <tx_api.h>

#define BYTE_POOL_SIZE 40000
UCHAR memory_area[BYTE_POOL_SIZE];
TX_BYTE_POOL byte_pool;

void initMemoryPool()
{
	tx_byte_pool_create(&byte_pool, "Byte pool 40K", memory_area, BYTE_POOL_SIZE);
}

void *calloc(size_t num, size_t size)
{
	return malloc(num * size);
}

void *malloc(size_t size)
{
	void *ptr = NULL;

	// CHAR *name;
	// ULONG available;
	// ULONG fragments;
	// UINT status = tx_byte_pool_info_get(&byte_pool, &name, &available, &fragments, NULL, NULL, NULL);
	// printf("%s - avail: %d, fragments: %d\n", name, available, fragments);

	if (size > 0)
	{
		uint8_t r = tx_byte_allocate(&byte_pool, &ptr, size, 1000);

		if (r != TX_SUCCESS)
		{
			return NULL;
		}
	} //else NULL if there was an error

	return ptr;
}

void *realloc(void *ptr, size_t size)
{
	// XXX TODO
	// make it fail for now
	return NULL;
}

void free(void *ptr)
{
	if (ptr)
	{
		tx_byte_release(ptr);
		ptr = NULL;
	}
}