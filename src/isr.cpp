#include "isr.hpp"
#include "pic.hpp"

InterruptDescriptor* const IDTAddress = (InterruptDescriptor*) 0x800;
IHandler* const ISRTableAddress = (IHandler*) 0x1000;
IHandler* const ISRTableEnd = (IHandler*) 0x2000;

void CDECL unhandledISR(uint32_t index, void* v)
{
	//I decided that I don't give a crap
	//about yo unhandled interrupts
	PIC_EndOfInterrupt(index);
	return;
	
	//Now here's some proper interrupt handling
	//In case you're interested
	/*
	(void)v;
	if (index == 0x27) //Spurious
	{
		PIC_EndOfInterrupt(index);
		return;
	}
	if (index == 0x2F)
	{
		PIC_EndOfInterrupt(index);
		return;
	}
	
	kprint("Unhandled interrupt number ");
	kprintnum(index);
	kprint("\nStack at ");
	kprintnum((uint32_t)v);
	kprint("\n");
	uint8_t* st = (uint8_t*)v;
	for (int i = 0; i < 128; i++)
	{
		kprintnum(st[i]);
		if ((i % 16) == 7)
			kprint("-");
		if ((i % 16) == 15)
			kprint("\n");
		
	}
	//System halt
	for(;;) asm("hlt");
	
	*/
}

//Division by zero
void CDECL div0(uint32_t index, void* v)
{
	(void)v;
	
	
	uint32_t* stk = (uint32_t*) v;
	stk = stk + 8; //skip the 8 integers pushed by pushad
	
	//stk now points to ip
	kprint("Div by zero at ");
	kprintnum(*stk);
	kprint("\n");
	
	//divs are usually 2 bytes in size;
	*stk = (*stk) + 2;
	
	
	
	PIC_EndOfInterrupt(index);
	return;
}

int ISRTableUsed = 0;


void ISR_SetIDT(InterruptServiceRoutine rtn, uint8_t index)
{
	InterruptDescriptor& id = IDTAddress[index];
	uint32_t func = (uint32_t)rtn;
	
	IHandler& isr = ISRTableAddress[ISRTableUsed];

	/*
		struct IHandler
		When assigned the correct values
		the memory representation matches
		the bytecode produced by the following assembly:

		pushad				;push registers
		cld					;clear direction 
		push esp			;argument 2 (stack pointer)
		push dword index	;argument 1
		call func			;call
		add esp 8			;reset stack
		popad				;pop registers
		iretd               ;interrupt return

		This is used for runtime interrupt
		handler generation.
	*/

	ISRTableUsed++;
	isr.entry = 0x6854FC60; //pushad, cld, push esp, immediate dword push op
	isr.param1 = index;		//operand for the push
	isr.callE8 = 0xE8;		//relative call op

	//The interrupt handler can be easily updated
	//by modifying the (relative) call operand
	isr.relativeAddress = func - (uint32_t)(&isr.relativeTo);
	
	isr.relativeTo = 0x6108C483; 	//add esp 8, popad
	isr.iret = 0xCF;				//iretd

	uint32_t ourisr = (uint32_t) &isr;
	//ourisr = (uint32_t) ISR0;

	kprint("\nISR ");
	kprintnum((uint8_t)index);
	kprint(" at ");
	kprintnum(ourisr);

	id.isrLow = ourisr & 0xFFFF;
	id.isrHigh = (ourisr >> 16) & 0xFFFF; 
	id.zero = 0;
	id.attributes = 0x8E;
	id.selector = 0x08;
}


void ISR_UpdateIDT(InterruptServiceRoutine rtn, uint8_t index)
{

	InterruptDescriptor& id = IDTAddress[index];
	uint32_t func = (uint32_t)rtn;
	uint32_t iadd = id.isrLow;
	iadd += ((uint32_t)id.isrHigh) << 16;

	IHandler* isr = (IHandler*) iadd;

	isr->relativeAddress = func - (uint32_t)(&isr->relativeTo);
}


void ISR_Init()
{

	kprint("\nISR TABLE: ");
	kprintnum((uint32_t)ISRTableAddress);
	kprint("\nIDT: ");
	kprintnum((uint32_t)IDTAddress);
	int isrCount = (uint32_t)ISRTableEnd - (uint32_t)ISRTableAddress;
	isrCount = isrCount / sizeof(IHandler);
	ISRTableUsed = 0;
	
	//Set handlers for unhandled
	for (int i = 0; i < 0x40; i++)
		ISR_SetIDT(&unhandledISR,i);


	IDTHeader.base = (uint32_t)IDTAddress;
	IDTHeader.size = ISRTableUsed*sizeof(InterruptDescriptor);
	IDT_LIDT();
	kprint("\nIDT HEADER: ");
	kprintnum((uint32_t)&IDTHeader);
	kprint("\nBASE: ");
	kprintnum(IDTHeader.base);
	kprint("\nLIMIT: ");
	kprintnum(IDTHeader.size);

	kprint("\n");

	ISR_UpdateIDT(&div0,0);
}

