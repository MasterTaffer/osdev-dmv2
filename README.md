# osdev DMV2 

Some kind of operating system level thing. Write this on a bootable device such as a USB driver or a floppy disk.
Boot with that device and this program will display the amazing DMV2 video in 320x200 accompanied with Opus audio through the PC speaker.

[About DMV2](https://github.com/MasterTaffer/dmv2)


## Compiling the system-thingy-kernel:

### What you need:

 - Cross compiler binutils targetting i686 elf
 - [Opus](http://opus-codec.org) source code (at least 1.1.3)
 - [LZMA SDK](http://www.7-zip.org/sdk.html) source code (7zTypes.h Alloc.c Alloc.h Compiler.h CpuArch.c CpuArch.h LzFind.c LzFind.h LzHash.h Lzma2Dec.c Lzma2Dec.h LzmaDec.c LzmaDec.h LzmaLib.c LzmaLib.h Precomp.h)
 - Netwide Assembler (NASM)

### 1. Compile opus
Preferably 1.1.3 with fixed math on and correct target settings. USE_ALLOCA is also recommended.
	
### 2. Compile bootloader
Either the LBA bootloader (lowbootlba.asm) or the CHS bootloader (lowboot.asm).
  Compile with NASM:
  
    nasm -f bin lowbootlba.asm -o lowbootlba
	
### 3. Compile rest of the system

All the code (*.c, *.cpp, *.asm) in "src" and "libc" must be compiled. The LZMA code should be compiled with the kernel. Kernel entry point must also be compiled:
	
    nasm -f elf32 kernel.asm -o kernel.o
	
### 4. Link the kernel

Use the linker.ld linker script. 
	
    i686-elf-gcc -T linker.ld -ffreestanding -nostdlib $OBJS -L. -lopus -lgcc
	

### 5. Concatenate the image

The boot image is structured as follows:
	
Bootloader (always 512 bytes) - Kernel (variable size) - Data (variable size) which must start at offset 174080.
	
One may achieve this structure with something like this:
	
    cat lowboot krnl > out.img
    dd if=/dev/zero of=out.img bs=1 count=1 seek=174079
    cat video.dmv2.lzma audio.rawopus >> out.img
	
And the image is ready for use. In some cases it is useful to resize the image file to the desired size (e.g. floppy image size of 1.44M.) 
	
    dd if=/dev/zero of=out.img bs=1 count=1 seek=1474559
	
### Getting the data into right format

The video must be [DMV2 encoded](https://github.com/MasterTaffer/dmv2) and compressed with LZMA using the lzmautil: compile it with the LZMA SDK.

The audio must simply be in raw containerless opus format, 24000hz single channel. If you compile the Opus SDK test utility (test_opus_encode.exe) it can be used.

