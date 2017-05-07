BITS 32
global kentry
kentry:

;call the main function
extern kmain
call kmain

;hopefully never return
;'cos we gonna halt here
cli
;write some bytes to the video memory
;they might not be visible
mov byte [0xB8000], 'H'
mov byte [0xB8002], 'A'
mov byte [0xB8004], 'L'
mov byte [0xB8006], 'T'
mov byte [0xB8008], '!'

mov byte [0xB8001], 0x1F
mov byte [0xB8003], 0x1F
mov byte [0xB8005], 0x1F
mov byte [0xB8007], 0x1F
mov byte [0xB8009], 0xF1

hlt
