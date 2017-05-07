#include "video.hpp"
#include <math.h>

//6x6 font, starting from ascii character 32
//higher bits -> rightmost pixels
//lowest index -> topmost pixels
//VideoLetters_6_6[1][0] contains the top row of '!' glyph:
//	VideoLetters_6_6[1][0] & 0x1 retrieves the top-left
//	pixel of the glyph
uint8_t VideoLetters_6_6[96][6]= 
{
{0, 0, 0, 0, 0, 0},
{3, 3, 1, 0, 1, 0},
{5, 5, 0, 0, 0, 0},
{10, 31, 10, 31, 10, 0},
{30, 5, 14, 20, 15, 0},
{17, 8, 4, 2, 17, 0},
{2, 5, 22, 9, 22, 0},
{1, 1, 0, 0, 0, 0},
{2, 1, 1, 1, 2, 0},
{1, 2, 2, 2, 1, 0},
{5, 2, 5, 0, 0, 0},
{4, 4, 31, 4, 4, 0},
{0, 0, 0, 2, 1, 0},
{0, 0, 31, 0, 0, 0},
{0, 0, 0, 0, 1, 0},
{4, 4, 2, 2, 1, 0},
{14, 17, 17, 17, 14, 0},
{6, 4, 4, 4, 14, 0},
{15, 16, 14, 1, 31, 0},
{15, 16, 12, 16, 15, 0},
{17, 17, 31, 16, 16, 0},
{31, 1, 15, 16, 15, 0},
{14, 1, 15, 17, 14, 0},
{31, 16, 8, 4, 2, 0},
{14, 17, 14, 17, 14, 0},
{14, 17, 30, 16, 14, 0},
{0, 2, 0, 2, 0, 0},
{0, 2, 0, 2, 1, 0},
{4, 2, 1, 2, 4, 0},
{0, 7, 0, 7, 0, 0},
{1, 2, 4, 2, 1, 0},
{3, 4, 2, 0, 2, 0},
{30, 25, 29, 1, 30, 0},
{14, 17, 17, 31, 17, 0},
{15, 17, 15, 17, 15, 0},
{30, 1, 1, 1, 30, 0},
{15, 17, 17, 17, 15, 0},
{31, 1, 31, 1, 31, 0},
{31, 1, 31, 1, 1, 0},
{31, 1, 29, 17, 15, 0},
{17, 17, 31, 17, 17, 0},
{31, 4, 4, 4, 31, 0},
{16, 16, 16, 17, 30, 0},
{17, 9, 7, 9, 17, 0},
{1, 1, 1, 1, 31, 0},
{17, 27, 21, 17, 17, 0},
{17, 19, 21, 25, 17, 0},
{14, 17, 17, 17, 14, 0},
{15, 17, 31, 1, 1, 0},
{14, 17, 17, 9, 22, 0},
{15, 17, 15, 9, 17, 0},
{14, 1, 14, 16, 14, 0},
{31, 4, 4, 4, 4, 0},
{17, 17, 17, 17, 14, 0},
{17, 17, 10, 10, 4, 0},
{17, 17, 21, 21, 10, 0},
{17, 10, 4, 10, 17, 0},
{17, 10, 4, 4, 4, 0},
{31, 8, 4, 2, 31, 0},
{3, 1, 1, 1, 3, 0},
{1, 1, 2, 2, 4, 0},
{3, 2, 2, 2, 3, 0},
{2, 5, 0, 0, 0, 0},
{0, 0, 0, 0, 31, 0},
{1, 2, 0, 0, 0, 0},
{0, 6, 5, 5, 10, 0},
{1, 1, 7, 5, 7, 0},
{0, 7, 1, 1, 7, 0},
{4, 4, 7, 5, 7, 0},
{0, 7, 7, 1, 7, 0},
{6, 1, 7, 1, 1, 0},
{10, 5, 7, 4, 6, 0},
{1, 1, 3, 5, 5, 0},
{1, 0, 1, 1, 1, 0},
{4, 0, 4, 5, 6, 0},
{1, 1, 5, 3, 5, 0},
{3, 2, 2, 2, 7, 0},
{0, 0, 15, 21, 21, 0},
{0, 0, 3, 5, 5, 0},
{0, 2, 5, 5, 2, 0},
{0, 3, 5, 7, 1, 0},
{10, 5, 7, 4, 12, 0},
{0, 5, 3, 1, 1, 0},
{0, 7, 3, 4, 7, 0},
{0, 2, 7, 2, 2, 0},
{0, 5, 5, 5, 7, 0},
{0, 17, 17, 21, 10, 0},
{0, 5, 5, 5, 2, 0},
{0, 5, 2, 2, 5, 0},
{0, 5, 5, 2, 2, 0},
{0, 7, 4, 2, 7, 0},
{6, 2, 3, 2, 6, 0},
{1, 1, 1, 1, 1, 0},
{3, 2, 6, 2, 3, 0},
{0, 0, 10, 5, 0, 0},
{0, 0, 0, 0, 0, 0}
};


uint8_t*  VideoDec_Blocks = (uint8_t *) 0x180000;
uint8_t*  VideoDec_PreRenderedBlocks = (uint8_t *) 0x182000;

size_t VideoDec_ReadVarInt(uint8_t* from, size_t* bytecount)
{
	size_t r = 0;
	char shl = 0;
	size_t count = 0;
	while (1)
	{
		int d = *from;
		count += 1;
		r += (d % 128) << shl;
		if (d < 128)
		{
			*bytecount = count;
			return r;
		}
		shl += 7;
		from += 1;
	}
}
#include "os.hpp"

size_t VideoDec_LoadGFrame(unsigned char* from)
{
	int reads = 40*25;
	size_t index = 0;
	size_t i2 = 0;
	while (reads > 0)
	{
		int f = from[index];
		
		
		index += 1;
		size_t addc;
		int reps = VideoDec_ReadVarInt(from+index,&addc);
		index += addc;
		

		for (int i = 0; i < reps; i++)
		{
			VideoDec_Blocks[i2] = f;
			i2++;
		}
		reads -= reps;
	}
	
	for (int i = 0; i < 40*25; i++)
		VideoDec_DrawBlock(i);
	return index;
}

int VideoDec_GradientDiff(int gval, int c)
{
	if (c == 3)
		return gval + 8;
	if (c == 1)
		return gval-1;
	if (c == 2)
		return gval+1;
	if (c == 4)
		return gval-3;
	if (c == 5)
		return gval+3;
	if (c == 6)
		return gval-5;
	if (c == 7)
		return gval+5;
	return gval;
}

void VideoDec_ApplyDBlock(size_t i, int b)
{
	uint8_t block = VideoDec_Blocks[i];
	
	uint8_t power = block & 0x7;
	uint8_t angle = (block >> 3) & 0xf;
	uint8_t dside = (block >> 7) & 0x1;
	
	uint8_t powc = b & 0x7;
	uint8_t angc = (b >> 3) & 0x7;
	uint8_t dsidec = (b >> 6) & 0x1;
	
	power = (power - powc);
	angle = VideoDec_GradientDiff(angle, angc);
	dside = (dside ^ dsidec) & 1;
	
	power = power & 0x7;
	angle = angle & 0xf;

	VideoDec_Blocks[i] = power + (angle << 3) + (dside << 7);
	VideoDec_DrawBlock(i);
}

const char sinetable[] =
{
	16 , 0 ,
	14 , 6 ,
	11 , 11 ,
	6 , 14 ,
	0 , 16 ,
	-6 , 14 ,
	-11 , 11 ,
	-14 , 6 ,
	-16 , 0 ,
	-14 , -6 ,
	-11 , -11 ,
	-6 , -14 ,
	0 , -16 ,
	6 , -14 ,
	11 , -11 ,
	14 , -6

};

const char angtable[] =
{
	16 , 0 ,
	17 , 7 ,
	15 , 15 ,
	7 , 17 ,
	0 , 16 ,
	-7 , 17 ,
	-15 , 15 ,
	-17 , 7 ,
	-16 , 0 ,
	-17 , -7 ,
	-15 , -15 ,
	-7 , -17 ,
	0 , -16 ,
	7 , -17 ,
	15 , -15 ,
	17 , -7
};

const char angtabledside[] =
{
	11 , 4 , -11 , 4 ,
	10 , 10 , -14 , 0 ,
	6 , 15 , -15 , -6 ,
	0 , 14 , -10 , -10 ,
	-4 , 11 , -4 , -11 ,
	-10 , 10 , 0 , -14 ,
	-15 , 6 , 6 , -15 ,
	-14 , 0 , 10 , -10 ,
	-11 , -4 , 11 , -4 ,
	-10 , -10 , 14 , 0 ,
	-6 , -15 , 15 , 6 ,
	0 , -14 , 10 , 10 ,
	4 , -11 , 4 , 11 ,
	10 , -10 , 0 , 14 ,
	15 , -6 , -6 , 15 ,
	14 , 0 , -10 , 10 ,
};

#define blocksize 8

void VideoDec_DrawBlock(size_t i)
{
	int bx,by;
	bx = i % 40;
	by = i / 40;
	
	bx *= blocksize;
	by *= blocksize;
	
	uint8_t* ourblock = VideoDec_PreRenderedBlocks + (VideoDec_Blocks[i] * 16);
	for (int y = 0; y < blocksize; y++)
	{
		int ty = by + y;
		
		char *t = VideoPixelMemory;
		if (ty % 2)
		{
			t += Video_InterleaveOffset;
		}
		ty = ty / 2;
		t += (bx / Video_PixelsPerByte) + (ty * Video_Pitch);
		
		t[0] = ourblock[0];
		t[1] = ourblock[1];
		ourblock += 2;
	}
}

void VideoDec_RenderBlock(uint8_t block, uint8_t* to)
{
	//A simple algorithm to draw the block
	
	
	int8_t pow = block & 0x7;
	uint8_t angl = (block >> 3) & 0xf;
	
	uint8_t dside = (block >> 7) & 0x1;
	uint8_t dsideblack = 0;
	
	int l1dir,l2dir,ld1dir,ld2dir;
	int l1, l2, ld1, ld2;
	//dside = 0;
	
	if (!dside)
	{
		angl *= 2;
		
		l1dir = angtable[angl];
		l2dir = angtable[angl+1];
		int power = pow * 2 - 7;
		l1 = l1dir * power * blocksize;
		l2 = l2dir * power * blocksize;
	}
	else
	{
		if (pow % 2)
			pow = 7 - pow / 2;
		else
			pow = pow / 2;
		
		if (pow >= 4)
		{
			angl = (angl+8) & 0xf;
			pow = pow - 4;
			dsideblack = 1;
		}
		
		pow = pow * 2;
		if (pow == 6)
			pow -= 1;
		
		angl *= 4;
		
		l1dir = angtabledside[angl+0];
		l2dir = angtabledside[angl+1];
		ld1dir = angtabledside[angl+2];
		ld2dir = angtabledside[angl+3];
		
		int power = pow * 2 - 7;
		l1 = l1dir * power * blocksize;
		l2 = l2dir * power * blocksize;
		ld1 = -ld1dir * power * blocksize;
		ld2 = -ld2dir * power * blocksize;
		
		
		//TO SAVE A SINGLE ADD
		ld1 = ld1 - l1;
		ld2 = ld2 - l2;
	}
	
	#define difex (15 * 7 * 2)
	
	int dify = (- difex * 4) + difex / 2 + l2;
	int basedifx = (- difex * 4) + difex / 2 + l1;
	for (int y = 0; y < blocksize; y++)
	{
		int difx = basedifx;
		
		for (int x = 0; x < blocksize; x++)
		{
			bool out = false;
			
			int dot = difx * l1dir + dify * l2dir;
			if (dot > 0)
				out = true;
			if (dside)
			{
				dot = (difx + ld1) * ld1dir + (dify + ld2) * ld2dir;
				if (dot <= 0)
					out = true;
				if (dsideblack)
					out = !out;
			}
			
			
			char bnum = x % Video_PixelsPerByte;
			if (bnum == 0)
				*to = 0;
			if (out)
				*to += (3 << (6 - bnum*2));
			if (bnum == 3)
				to++;
			
			difx += difex;
		}
		dify += difex;
	}
}


void VideoDec_PreRenderBlocks()
{
	for (int state = 0; state < 256; state += 1)
	{
		uint8_t b = state;
		VideoDec_RenderBlock(b,VideoDec_PreRenderedBlocks+(state*16));
	}
	
}


void VideoDec_Init()
{
	VideoDec_PreRenderBlocks();
	VideoDec_Reset();
}
size_t VideoDec_ApplyDFrame(unsigned char* from)
{

	int blocks = 0;
	size_t index = 0;
	size_t xrepeatMask = 0x80;
	while (blocks < 40*25)
	{
		size_t p = from[index];
		size_t xamount = 1;
		if (p & xrepeatMask)
		{
			size_t bytes = 0;
			xamount = VideoDec_ReadVarInt(from+index+1,&bytes) + 1;
			index += bytes;
		}
		index += 1;
		
		
		p = p & 0x7f;
		if (p == 0)
		{
			blocks += xamount;
		}
		else
		{
			while (xamount > 0)
			{
				VideoDec_ApplyDBlock(blocks,p);
				xamount--;
				blocks++;
			}
		}
	}
	
	if (blocks != 40*25)
	{
		//Throw an error or something
		//But we don't care, just go on
		
		//What's the worst that could happen?
	}
	return index;
}

unsigned char* VideoDec_Frames;
size_t VideoDec_FrameCount = 6570;
static size_t videooff = 0;
static size_t videoframe = 0;


void VideoDec_NextFrame()
{
	if (videoframe >= VideoDec_FrameCount)
		return;
	if (videoframe == 0)
		videooff += VideoDec_LoadGFrame(VideoDec_Frames + videooff);
	else
		videooff += VideoDec_ApplyDFrame(VideoDec_Frames + videooff);
	videoframe++;
}
void VideoDec_Reset()
{
	videooff = 0;
	videoframe = 0;
}