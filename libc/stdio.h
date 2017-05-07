#ifndef IRLIBC_STDIO_GUARD
#define IRLIBC_STDIO_GUARD
#define FILE int
#define stderr ((FILE*)0x1)
#define EXIT_FAILURE 0x1

#define fprintf (void)

#define exit (void)
#endif //IRLIBC_STDIO_GUARD

