#ifndef IRLIBC_STRING_GUARD
#define IRLIBC_STRING_GUARD
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy(void * s1, const void * s2, size_t n);
void *memmove(void * s1, const void * s2, size_t n);
void *memset(void *str, int c, size_t n);
size_t strlen(const char *s);
#ifdef __cplusplus
}
#endif


#endif //IRLIBC_STRING_GUARD

