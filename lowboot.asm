org 0x7C00

bits 16

jmp short start
oem db "LowBootC"
secsPerTrack: dw 18
heads: dw 2
lastwrite: dd 0x100000
bootdev: db 0
%define rdcount 1

%define DesiredVideoMode 0x04
%define FirstDataSector 340
%define LastDataSector 2880

start:

	;setup stack and segments
	mov ax, 0x800
	cli
	mov ss, ax
	mov sp, 0x8000
	sti

	mov ax, 0x0
	mov ds, ax

	;store bootdevice
	mov [bootdev], dl

	;set video mode
	mov al, DesiredVideoMode
	mov ah, 0x0
	int 10h

	;disable cursor
	mov ah,01h
	mov ch, 0b0010000
	mov cl, 0
	int 10h

	;fast A20
	in al, 0x92
	or al, 2
	out 0x92, al

	;read data sectors
	mov ax, 0x1000
	mov es, ax

	mov ax, FirstDataSector
.again:
	push ax
	call fread

	mov ax, es
	add ax, 0x20
	cmp ax, 0x9000
	jb .dont 

	call procs

	mov ax, 0x1000
.dont:
	mov es,ax
	pop ax
	inc ax
	cmp ax,LastDataSector
	jae lkern
	jmp .again

lkern:
	call procs

	mov ax, 0x1000
	mov es, ax

	mov ax, 1
.again:
	push ax
	call fread

	mov ax, es
	add ax, 0x20
	mov es, ax
	pop ax
	inc ax
	cmp ax, FirstDataSector
	jb .again
	jmp done

errarr:
	mov ah, 0xe
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
	
fread:
	call calculateCHS
	pushad
	int 13h
	jc .tagain
	mov ah, 0xe
	mov al, [es:bx]
	mov bx, 1
	int 10h
	popad
	ret
	.tagain:
	popad
	pushad
	call freset
	popad
	pushad
	int 13h
	jc .errt
	popad
	ret
	.errt:

	mov al, ah
	mov ah, 0xe
	add al, 'A'
	mov bx,1
	int 10h
	popad
	ret

;r
freset:
	push ax
	mov ax,0
	int 13h
	jc errarr
	pop ax
	ret

;http://wiki.osdev.org/ATA_in_x86_RealMode_(BIOS)#Converting_LBA_to_CHS
;Prepares registers for int 0x13 CHS floppy read
;This only works for devices with less than 256 cylinders
;Input: ax = logical sector
calculateCHS:
	
	mov dx, 0
	;Division: LBA / secsPerTrack
	div word [secsPerTrack]
	;Result in ax, remainder in dx
	;Sector = (LBA % secsPerTrack) + 1
	mov cl, dl
	add cl, 1
	
	;(LBA / secsPerTrack) /  heads
	mov dx, 0
	div word [heads]
	;Cylinder = result
	mov ch, al
	;Head = remainder
	mov dh, dl
	
	mov ah, 2
	mov al, 1 ;one sector at a time
	mov bx, 0 
	mov dl, [bootdev]
	
	ret

done:
;stop floppy drive
	mov edx,0x3F2
	mov al,0x0C
	out dx,al 
	
%include "protected.asm"
times 510 - ($-$$) db 0
dw 0xaa55