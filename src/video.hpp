#pragma once
#include "stdint.h"
#include "stddef.h"

extern uint8_t VideoLetters_8_13[95][13];
extern uint8_t VideoLetters_6_6[96][6];
extern char * const VideoPixelMemory;
extern char * const VideoCharMemory ;
extern int VideoCurrentRow;
extern int VideoCurrentColumn;

extern const int VideoMaxRow;
extern const int VideoMaxColumn;

extern const int Video_CharacterHeight;
extern const int Video_CharacterWidth;

extern const int Video_Width;
extern const int Video_Height;

extern const int Video_Pitch;
extern const int Video_PixelsPerByte;
extern const int Video_PixelMask;

extern const bool VideoTextMode;
extern const bool Video_InterleavedScanlines;
extern const int Video_InterleaveOffset;

extern size_t VideoDec_FrameCount;
extern unsigned char* VideoDec_Frames;


size_t VideoDec_LoadGFrame(unsigned char* from);
void VideoDec_ApplyDBlock(size_t i, int b);
void VideoDec_DrawBlock(size_t i);
size_t VideoDec_ApplyDFrame(unsigned char* from);
void VideoDec_NextFrame();
void VideoDec_Reset();
void VideoDec_Init();