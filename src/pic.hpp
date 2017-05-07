#pragma once
#include "os.hpp"

//Thanks osdev:

inline void PIC_EndOfInterrupt(uint32_t isr_index)
{
	if(isr_index >= 8)
		outb(0xA0,0x20);
	outb(0x20,0x20);
}
