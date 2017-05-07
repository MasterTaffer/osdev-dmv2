#include "os.hpp"
#include "pic.hpp"

#include "stdlib.h"

#include "arch.hpp"
#include "opus.hpp"

#include "sampled.hpp"
#include "video.hpp"

#include <math.h>
#include <string.h>

char * const VideoPixelMemory = (char*) 0xB8000;
char * const VideoCharMemory = (char*) 0xB8000;
int VideoCurrentRow = 0;
int VideoCurrentColumn = 0;

const int VideoMaxRow = 33;
const int VideoMaxColumn = 53;

const int Video_CharacterHeight = 6;
const int Video_CharacterWidth = 6;

const int Video_Width = 320;
const int Video_Height = 200;

const int Video_Pitch = 80;
const int Video_PixelsPerByte = 4;
const int Video_PixelMask = 0xC0;

const bool VideoTextMode = false;
const bool Video_InterleavedScanlines = true;
const int Video_InterleaveOffset = 0x2000;

void Video_PutPixel(char c, uint16_t x, uint16_t y)
{
	char bnum = x % Video_PixelsPerByte;
	char *t = VideoPixelMemory;
	if (Video_InterleavedScanlines)
	{
		if (y % 2)
		{
			t += Video_InterleaveOffset;
		}
		y = y / 2;
	}		
	t = t + ((x / Video_PixelsPerByte) + (y * Video_Pitch));
	bnum *= 2;
	*t = ((*t) & ~(Video_PixelMask >> bnum)) | (c << (6 - bnum));
}


void Video_PlotCharacter(char c, uint8_t x, uint8_t y)
{
	if (VideoTextMode)
	{
		VideoCharMemory[x + y * VideoMaxColumn] = c;
		VideoCharMemory[(x + y * VideoMaxColumn) + 1] = 0x17;
	}
	else
	{
		//Works only for Video_CharacterWidth <= 8
		size_t basex = x * Video_CharacterWidth;
		size_t basey = y * Video_CharacterHeight;
		for (size_t i = basex; i < basex+Video_CharacterWidth; i++)
		for (size_t j = basey; j < basey+Video_CharacterHeight; j++)
		{
			size_t cx = i - basex;
			size_t cy = j - basey;
			//cy *= 12;
			//cy /= 8;
			
			
			bool on;
			if (cy >= Video_CharacterHeight)
				on = false;
			else
				on = (VideoLetters_6_6[c-32][cy]  >> cx) & 0x1;
			
			Video_PutPixel(on * 0x3, i, j);

		}
	}
}

void Video_ScrollUp()
{
	char* f;
	char* t;
	int cnt;
	if (VideoTextMode)
	{
		f = VideoCharMemory + (VideoMaxColumn * 2);
		t = VideoCharMemory;
		cnt = VideoMaxColumn * 2 * (VideoMaxRow - 1);
		memcpy(t,f,cnt);
	}
	else
	{
		if (Video_InterleavedScanlines)
		{
			uint32_t rowoff = ((Video_Width * Video_CharacterHeight / 2) / Video_PixelsPerByte );
			cnt = Video_Width * ((Video_Height - Video_CharacterHeight) / 2) / Video_PixelsPerByte;
			
			f = VideoPixelMemory + rowoff;
			t = VideoPixelMemory;
			
			memcpy(t,f,cnt);
			
			f = VideoPixelMemory + Video_InterleaveOffset + rowoff;
			t = VideoPixelMemory + Video_InterleaveOffset;
			memcpy(t,f,cnt);
			
			uint32_t restcnt = (Video_Width * Video_Height / 2) / Video_PixelsPerByte;
			restcnt -= cnt;
			memset(VideoPixelMemory + cnt,0,restcnt);
			memset(VideoPixelMemory + Video_InterleaveOffset + cnt,0,restcnt);
			
		}
		else
		{
			f = VideoPixelMemory + ((Video_Width * Video_CharacterHeight) / Video_PixelsPerByte );
			t = VideoPixelMemory;
			cnt = Video_Width * (Video_Height - Video_CharacterHeight) / Video_PixelsPerByte;
			memcpy(t,f,cnt);
		}
		
	}
}

void kprint(const char* c)
{
	const int termWidth = VideoMaxColumn;
	const int termHeight = VideoMaxRow;
	const char bw = 0x17;
	int offs = VideoCurrentRow * termWidth + VideoCurrentColumn;
	while (*c)
	{
		if (*c == '\n')
		{
			VideoCurrentRow++;
			VideoCurrentColumn = 0;
			offs = VideoCurrentRow * termWidth + VideoCurrentColumn;
		}
		else
		{
			Video_PlotCharacter(*c,VideoCurrentColumn,VideoCurrentRow);
			//VideoCharMemory[offs*2] = *c;
			//VideoCharMemory[offs*2 + 1] = bw;

			++VideoCurrentColumn;
			++offs;

			if (VideoCurrentColumn >= termWidth)
			{
				VideoCurrentRow++;
				VideoCurrentColumn = 0;
			}
		}

		if (VideoCurrentRow >= termHeight)
		{
			Video_ScrollUp();
			VideoCurrentRow -= 1;
			offs = VideoCurrentRow * termWidth + VideoCurrentColumn;

		}
		++c;
		
	}
}

void kprint(const char* c, size_t len)
{
	const int termWidth = VideoMaxColumn;
	const int termHeight = VideoMaxRow;
	const char bw = 0x17;
	int offs = VideoCurrentRow * termWidth + VideoCurrentColumn;
	while (len)
	{
		if (*c == '\n')
		{
			VideoCurrentRow++;
			VideoCurrentColumn = 0;
			offs = VideoCurrentRow * termWidth + VideoCurrentColumn;
		}
		else
		{
			Video_PlotCharacter(*c,VideoCurrentColumn,VideoCurrentRow);
			
			++VideoCurrentColumn;
			++offs;

			if (VideoCurrentColumn >= termWidth)
			{
				VideoCurrentRow++;
				VideoCurrentColumn = 0;
			}
		}

		if (VideoCurrentRow >= termHeight)
		{
			Video_ScrollUp();
			VideoCurrentRow -= 1;
			offs = VideoCurrentRow * termWidth + VideoCurrentColumn;

		}
		++c;
		len--;
	}
}


extern "C"
void kmain()
{
	for (int j = 0; j < 200; j++)
		for(int i = 0; i < 320; i++)
		{
			if (((i + j)/2) % 2)
				Video_PutPixel(3,i,j);
			else
				Video_PutPixel(0,i,j);
		}
	
	
	
	kprint("Initializing...\n");
	
	kprint("More initializin...\n");
	
	
	ISR_Init();

	char* datapos = (char*) 0x100000;
	int k = *((int*)datapos);
	int firstDataLen = k;
	char* firstDataPos = datapos + 4;
	
	datapos = datapos + (k + 4);
	int secondDataLen = *((int*)datapos);
	char* secondDataPos = datapos + 4;
	
	kprint("First data len: ");
	kprintnum<uint32_t>(firstDataLen);
	kprint("\n");
	
	kprint("Second data len: ");
	kprintnum<uint32_t>(secondDataLen);
	kprint("\n");
	
	char* outpos = (char*)0x400000;
	int ret;
	size_t stackused;
	size_t memused = 0;
	memused = unpackFrom(firstDataPos,firstDataLen, outpos, &ret, &stackused);
	VideoDec_Frames = (unsigned char*)outpos;
	
	outpos += memused;
	kprint("Unpacked: ");
	kprintnum(memused);
	
	kprint(" - ");
	kprintnum(ret);
	kprint("\n");
	memused = unOPUSFrom(secondDataPos,secondDataLen, outpos, &ret, &memused);
	outpos += memused;
	
	kprint("UnOPUS'd: ");
	kprintnum(memused);
	kprint(" - ");
	kprintnum(ret);
	kprint("\n");

	
	outb(0x70, inb(0x70)&0x7F);

	
	VideoDec_Init();
	kprint("Makin Dat sound work...\n");
	Sampled_Init();
	kprint("Runnin wild!\n");

	asm("sti");
	
	
	Sampled_Run();
	
	return;
}

