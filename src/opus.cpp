#include "stdlib.h"
#include "string.h"
#include "opus.hpp"
#include "os.hpp"
#include <opus.h>
#include "sampled.hpp"

static uint32_t char_to_int(char ch[4])
{
    return static_cast<uint32_t>(static_cast<unsigned char>(ch[0])<<24) |
        static_cast<uint32_t>(static_cast<unsigned char>(ch[1])<<16) |
        static_cast<uint32_t>(static_cast<unsigned char>(ch[2])<< 8) |
        static_cast<uint32_t>(static_cast<unsigned char>(ch[3])<< 0);
}

char freedata[0x6000];
size_t unOPUSFrom(char* from, size_t size, char* to, int* retok, size_t* memused)
{
	*((size_t*)MemoryStackBase) = 0;
	int err;
	
	void* v1 = malloc(64);
	void* v2 = malloc(64);
	void* v3 = malloc(64);
	void* v4 = malloc(64);
	kprintnum<uint32_t>((uint32_t)v1);kprint("\n");
	kprintnum<uint32_t>((uint32_t)v2);kprint("\n");
	kprintnum<uint32_t>((uint32_t)v3);kprint("\n");
	kprintnum<uint32_t>((uint32_t)v4);kprint("\n");
	
	OpusDecoder* decoder = (OpusDecoder*) freedata;
	opus_decoder_init(decoder, 24000, 1);
	
	kprint("Decoder at ");
	kprintnum<uint32_t>((uint32_t)decoder);
	kprint(" used memory ");
	kprintnum<uint32_t>(opus_decoder_get_size(1));
	kprint("\n");

	const size_t max_frame_size = 0x4000;
	opus_int16* out = (opus_int16*) (to);
	unsigned char* formattedOut = (unsigned char*) (to+max_frame_size);
	size_t formattedOutPos = 0;
	size_t currentPos = 0;
	int cycles = 0;
	unsigned long ret = 0;

	int maxk = 0;
	int mink = 999999;
	do
	{
		//size_t frame_size = opus_decode(decoder, cbits, nbBytes, out, max_frame_size, 0);
		
		char ch[4] = {0};
		
		memcpy(ch,from+currentPos,4);
		currentPos += 4;

        uint32_t len = char_to_int(ch);

        memcpy(ch,from+currentPos,4);
		currentPos += 4;
		
		
        const uint32_t enc_final_range = char_to_int(ch);

        int output_samples;
		
		
		cycles++;
		if (len != 0)
			output_samples = opus_decode(decoder,(unsigned char*)(from+currentPos),len,out,max_frame_size,0);
		else
			output_samples = opus_decode(decoder,0,len,out,max_frame_size,0);

		currentPos += len;
		
		
		if (cycles % 20 == 0)
		{
			kprintnum(currentPos);
			kprint(" / ");
			kprintnum(size);
			kprint(", mem ");
			kprintnum(*((size_t*)MemoryStackBase));
			kprint("\n");
			
			kprintnum(mink);
			kprint(" - ");
			kprintnum(maxk);
			kprint("\n");
			
			maxk = 0;
			mink = 999999;
			
		}
        if(output_samples>0)
        {
            for(int i=0; i < output_samples; i++)
            {
                short s;
                s = out[i];
				int k = s;
				k += 32767;
				k /= 350;
				k += 3;
				if (k < mink)
					mink = k;
				if (k > maxk)
					maxk = k;
				unsigned char c = k;
				formattedOut[formattedOutPos] = c;
				formattedOutPos++;
            }
        }
        else
        {
			kprint("Opus Error: ");
			kprint(opus_strerror(output_samples));
			kprint("\n");
        }
		
		*((size_t*)MemoryStackBase) = 0;
		
	} while(currentPos < size);
	
	
	Sampled_SampleCount = formattedOutPos;
	Sampled_Samples = formattedOut;

	*memused = *((size_t*)MemoryStackBase);
	
	*((size_t*)MemoryStackBase) = 0;
	*retok = 0;
	return formattedOutPos;
}