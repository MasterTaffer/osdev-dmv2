#include "stdlib.h"

void *memcpy(void * s1, const void * s2, size_t n)
{
	char * restrict c1 = s1;
	const char * restrict c2 = s2;
	while (n)
	{
		*c1 = *c2;
		++c2;
		++c1;
		--n;
	}
	return s1;
}

void *memset(void *str, int c, size_t n)
{
	char * c1 = str;
	while (n)
	{
		*c1 = c;
		++c1;
		--n;
	}
	return str;
}


void *memmove(void *s1, const void *s2, size_t n)
{
	char* bg = s2;
	char* c1 = s1;
	char * c2 = (char*) s2;
	size_t dif = c1 - c2;
	size_t on = n;
	int overlap = 0;
	size_t overoff = 0;
	while (n)
	{
		if (n > dif)
		{
			*c1 = (*c2) ^ (*c1);
			overlap = 1;
		}
		else
		{
			if (overlap == 0)
				*c1 = *c2;
			else
			{
				char tk = (*c2) ^ (bg[overoff]);
				(*c2) = (*c2) ^ tk;
				(*c1) = tk;
				overoff++;
			}
		}
		++c2;
		++c1;
		--n;
	}
	return s1;
}

size_t strlen(const char *s)
{
	size_t count = 0;
	while (*(s++))
		count++;
	return count;
}

void free(void* ptr)
{
}
void* malloc(size_t size)
{
	size_t* b = (size_t*) 0xB8000;
	*b = (*b) + 1;
	size_t* off = (size_t*)MemoryStackBase;
	void* v = (void*)(MemoryStackBase + 4 + *off);
	*off += size; 
	return v;
	
}
void* alloca(size_t size)
{
	return malloc(size);
}
void* realloc(void* ptr, size_t size)
{
	void* n = malloc(size);
	memcpy(n,ptr,size);
	return n;
}
