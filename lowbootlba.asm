org 0x7C00

bits 16

jmp short start
oem db "LowBootL"
lastwrite: dd 0x100000
sectors: dw 2880
bootdev: db 0

%define DesiredVideoMode 0x04
%define FirstDataSector 340
%define LastDataSector 5200

start:

	;setup stack and segments
	mov ax, 0x800
	cli
	mov ss, ax
	mov sp, 0x8000

	mov ax, 0x0
	mov ds, ax
	sti

	test dl, 0x70
	jz .ovchg
	mov dl, 0x80
	.ovchg:

	mov [bootdev], dl

	;set video mode
	mov al, DesiredVideoMode
	mov ah, 0x0
	int 10h

	;fast A20
	in al, 0x92
	or al, 2
	out 0x92, al

	mov ax, FirstDataSector
	xor ecx, ecx
	mov dx, 0x1000
again:

	push ax
	push dx

	mov bx, 1
	mov cx, dx
	call nread

	pop dx

	add dx, 0x20
	cmp dx, 0x9000
	jb .dont

	call procs

	xor ecx, ecx
	mov dx, 0x1000
.dont:
	pop ax

	add ax, 1
	cmp ax, LastDataSector
	jae lkern
	jmp again

lkern:
	call procs

	mov ax, 1
	mov cx, 0x1000
.agn:

	mov bx, 1
	pushad
	call nread
	popad
	add cx, 0x20
	inc ax
	cmp ax, 400
	jl .agn

	jmp donewww


errarr:
	mov al,ah
	add al, 'A'
	mov ah, 0xe
	int 10h
	mov al,'e'
	int 10h
	cli
	hlt


procs:
	
	push eax
	mov eax, dword [lastwrite]
	mov edi, eax
	add eax, 0x80000
	mov dword [lastwrite], eax
	pop eax
	
	call protected_copy
	
	ret

align 4
datap:
			db	0x10
			db	0
blocks:		dw	1
target_b:	dw	0
target_s:	dw	0
readlba:	dd	1
			dd	0
;read using the lba read
nread:
	mov word[readlba], ax
	mov word[target_s], cx
	mov word[blocks], bx
	
	mov si, datap
	mov ah, 0x42
	mov dl, [bootdev]
	int 13h
	jc errarr
	mov es, cx
	mov al, [es:bx]
	mov ah, 0xe
	int 10h

		
	ret

donewww:
%include "protected.asm"
times 510 - ($-$$) db 0
dw 0xaa55