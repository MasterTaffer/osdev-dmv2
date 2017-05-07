BITS 32
%macro IHandler 2
	pushad
	cld
	push dword esp
	push dword %2
	call %1
	add esp, 8
	popad
	iret
%endmacro

%macro DummyIHandler 0
	add byte [0xB8000],1
	iret
%endmacro

extern IDTAddress


section .data

global IDTHeader
IDTHeader:
.limit: dw 0x100
.base: dd 0xDEADBEEF

section .text


global IDT_LIDT
IDT_LIDT:
	lidt [IDTHeader]
	ret


global ISR0
ISR0: DummyIHandler
ISR1: IHandler 0xDEAD1000, 0xDEADBEEF