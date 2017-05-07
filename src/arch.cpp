#include "stdlib.h"
#include "os.hpp"
#include "string.h"

#include <LzmaLib.h>


size_t unpackFrom(char* from, size_t size, char* to, int* retok, size_t* memused)
{
	*((size_t*)MemoryStackBase) = 0;

    size_t destlen = *((size_t*)from);
    size_t propssize = LZMA_PROPS_SIZE;
	kprint("Got us some data:");
	kprintnum(destlen);
	kprint("\n");
    unsigned char* dest = (unsigned char*)to;

    size_t inlen = size-4-propssize;

    from += 4;

    *retok = LzmaUncompress(dest, &destlen,((unsigned char*)from)+LZMA_PROPS_SIZE,&inlen,(unsigned char*)from, propssize);
    *memused = *((size_t*)MemoryStackBase);
	*((size_t*)MemoryStackBase) = 0;
	
    return destlen;
}