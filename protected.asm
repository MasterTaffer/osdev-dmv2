%macro protected_call 1
	;Set the protected mode bit
	mov eax, cr0
	or al, 1
	mov cr0, eax
	;And jump
	jmp 0x08:%1
%endmacro

BITS 16
	call protected_prep
	protected_call protected

;copy data into +1M area
protected_copy:
	call protected_prep
	protected_call proccopy

;Prepare CPU for protected mode
protected_prep:
	;Disable interrupts
	cli
	
	;Disable non maskable interrupts
	in al, 0x70
	or al, 0x80
	out 0x70, al
	
	mov ax,0
	mov ds,ax
	;Load Global Descriptor Table
	lgdt [gdtdesc]
	ret


;Protected copy: move 0x80000 bytes from
;0x10000 to edi
BITS 32
proccopy:

	mov ax,0x10
	mov ds,ax
	mov es,ax
	mov ecx, 0x80000
	mov esi, 0x10000
	rep movsb
	;return to real mode
	jmp 0x18:backTo16b

backTo16b:
BITS 16
	;set segments to real mode segments
	;not sure if it's necessary
	mov ax,0x20
	mov ds,ax
	mov es,ax
	
	;disable protected mode
	mov eax, cr0
	and eax, 0xFFFFFFFE
	mov cr0, eax
	
	;one last jmp to seal the deal
	jmp 0:backToReal

backToReal:
	mov ax,0
	mov ds,ax
	mov es,ax

	sti
	;stack is balanced: just return
	ret

BITS 32

protected:
	;prepare all the segments
	mov ax,0x10
	mov ds,ax
	mov es,ax
	mov gs,ax
	mov fs,ax
	
	;get the old real mode stack segment
	xor ebx,ebx
	mov bx,ss
	
	;and set it to selector 0x10 as well
	mov ss,ax
	
	;multiply the old one by 0x10
	mov eax, 0x10
	mul ebx
	
	;esp now points to the old stack
	add esp,eax
	
	;jump to our kernel entry point
	jmp 0x8:0x10000

;http://wiki.osdev.org/GDT
gdtdesc:
dw gdt_end - gdt - 1
dd gdt
gdt:
;Null
dw 0x0000 		;limit 0-15
dw 0x0000 		;base 0-15
db 0x00			;base 16-23
db 0b00000000	;access
db 0b00000000	;flags & limit 16-19
db 0x00			;base 24-31

;Code Ring 0
dw 0xFFFF 		;limit 0-15
dw 0x0000 		;base 0-15
db 0x00			;base 16-23
db 0b10011010	;access
db 0b11001111	;flags & limit 16-19
db 0x00			;base 24-31

;Data Ring 0
dw 0xFFFF 		;limit 0-15
dw 0x0000 		;base 0-15
db 0x00			;base 16-23
db 0b10010010	;access
db 0b11001111	;flags & limit 16-19
db 0x00			;base 24-31

;Code 16 Bit
dw 0xFFFF 		;limit 0-15
dw 0x0000 		;base 0-15
db 0x00			;base 16-23
db 0b10011010	;access
db 0b00001111	;flags & limit 16-19
db 0x00			;base 24-31

;Data 16 Bit
dw 0xFFFF 		;limit 0-15
dw 0x0000 		;base 0-15
db 0x00			;base 16-23
db 0b10010010	;access
db 0b00001111	;flags & limit 16-19
db 0x00			;base 24-31

gdt_end: