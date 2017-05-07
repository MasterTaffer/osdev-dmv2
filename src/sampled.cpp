#include "sampled.hpp"
#include "isr.hpp"
#include "pic.hpp"
#include "os.hpp"
#include "irfs.hpp"
#include "video.hpp"

#define SOUND_INTERRUPTS_PER_VIDEO_FRAME 800

uint8_t* Sampled_Samples = (uint8_t*) 0x204000;
volatile uint32_t sampleoff = 0;
uint32_t Sampled_SampleCount = 10;
volatile uint32_t sampleDivisor = 4;
volatile uint32_t sampleMul = 1;
volatile uint32_t sampleAdd = 4;
volatile uint32_t useAlgo = 0;
volatile bool songReset = false;
volatile uint32_t soundints = 0;

int sampleSpeed = 1;

void PCSpeaker_LinkPit()
{
	uint8_t tmp;
	tmp = inb(0x61);
	if (tmp != (tmp | 3))
	{
		outb(0x61, tmp | 3);
	}
}
void PCSpeaker_Strobe(uint8_t amount)
{
	uint16_t taram = amount;
	taram += sampleAdd;
	taram = taram / sampleDivisor;
	outb(0x42, taram & 0xFF);
	outb(0x42, (taram >> 8 ) & 0xFF);
}

void PCSpeaker_UnLinkPit()
{
	uint8_t tmp = inb(0x61) & 0xFC;
	outb(0x61, tmp);
}

void CDECL Sampled_ISR(uint32_t index, void* v)
{
	(void)v;
	if (sampleoff >= Sampled_SampleCount)
	{
		sampleoff = 0;
		songReset = true;
	}
	PCSpeaker_Strobe(Sampled_Samples[sampleoff]);
	sampleoff++;
	soundints++;
	PIC_EndOfInterrupt(index);
}

void Sampled_Init()
{
	VideoDec_Reset();
	ISR_UpdateIDT(&Sampled_ISR, 0x8);

	kprint("\nStarting Sampled PIT...");
	uint8_t pitcmd = 0x34;
	outb(0x43,pitcmd);

	//for (about) 8000 hz: 149
	//for (about) 44100 hz: 27
	//for (about) 22050 hz: 54
	
	//But we want 24khz
	
	//for (about) 24000 hz: 50 (1193181 / 50 == 23863)
	//this means we are running about 0.994x
	//the normal speed of the track:
	//the sound plays about 10 cents under the normal pitch 
	uint16_t pitreload = 50;
	outb(0x40,pitreload & 0xFF);
	outb(0x40,(pitreload >> 8) & 0xFF);
	
	outb(0x43, 0xB0);
	outb(0x42, 0);
	outb(0x42, 0);
	
	PCSpeaker_LinkPit();
}

void Sampled_Run()
{
	soundints = 0;
	sampleDivisor = 4;
	sampleMul = 1;
	sampleAdd = 4;
	soundints += SOUND_INTERRUPTS_PER_VIDEO_FRAME;
	
	for(;;)
	{
		//Sleep and wait for interrupts
		asm volatile ("hlt");
		if (soundints >= SOUND_INTERRUPTS_PER_VIDEO_FRAME)
		{
			if (songReset)
			{
				songReset = false;
				VideoDec_Reset();
				VideoDec_NextFrame();
			}
			else
			{
				VideoDec_NextFrame();
			}
			soundints -= SOUND_INTERRUPTS_PER_VIDEO_FRAME;
		}
		
	}
}