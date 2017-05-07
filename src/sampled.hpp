#pragma once
#include "stdint.h"
#include "stddef.h"

void Sampled_Init();
void Sampled_Run();
extern uint8_t* Sampled_Samples;
extern uint32_t Sampled_SampleCount;