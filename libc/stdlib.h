#ifndef IRLIBC_STDLIB_GUARD
#define IRLIBC_STDLIB_GUARD
#include <stddef.h>
#define MemoryStackBase 0x80000

#ifdef __cplusplus
extern "C" {
#endif


void* realloc(void* ptr, size_t size);
void free(void* ptr);
void* malloc(size_t size);
void *alloca(size_t size);

#ifdef __cplusplus
}
#endif

#endif //IRLIBC_STDLIB_GUARD

