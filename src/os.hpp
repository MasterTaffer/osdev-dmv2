#pragma once
#include <stdint.h>
#define Packed __attribute__ ((__packed__))
#define CDECL __attribute__((__cdecl__))

//Thanks osdev
//http://wiki.osdev.org/Inline_Assembly/Examples
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void kprint(const char* c);


template <typename T>
void gprintnum(T nm, void(*fun)(const char*))
{
	int k = sizeof(T) * 2;
	for (int i = k-1; i >= 0; i--)
	{
		char masked = (nm >> (i * 4)) & 0x0F;
		char cca[2] = {0,0}; 
		if (masked <= 9)
		{
			cca[0] = '0' + masked;
		}
		else
		{
			cca[0] = 'A' + masked - 10;
		}
		fun(cca);
	}
}
template <typename T>
void kprintnum(T nm)
{
	gprintnum(nm,kprint);
}


void ISR_Init();