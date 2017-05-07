#pragma once
#include "os.hpp"

typedef void ( *InterruptServiceRoutine)(uint32_t, void*) CDECL;

struct Packed InterruptDescriptor
{
   uint16_t isrLow;
   uint16_t selector;
   uint8_t zero;
   uint8_t attributes;
   uint16_t isrHigh;
};

struct Packed IHandler
{
	uint32_t entry;
	uint32_t param1;
	uint8_t callE8;
	uint32_t relativeAddress;
	uint32_t relativeTo;
	uint8_t iret;
};

extern "C" InterruptDescriptor* const IDTAddress;
extern "C" IHandler* const ISRTableAddress;
extern "C" IHandler* const ISRTableEnd;

extern "C" void IDT_LIDT();
extern "C" void ISR0();

void ISR_SetIDT(InterruptServiceRoutine rtn, uint8_t index);
void ISR_UpdateIDT(InterruptServiceRoutine rtn, uint8_t index);
void ISR_Init();


struct Packed InterruptDescriptorTableHeader 
{
	uint16_t size;
	uint32_t base;
};

extern "C" InterruptDescriptorTableHeader IDTHeader;
