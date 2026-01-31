//Base includes
#include <mednafen/mednafen.h>
#define MODULENAMESPACE pcsxr
#define SECONDARYINCLUDE
#include <module_helper.h>
using namespace pcsxr;

#include "stdafx.h"
#include "SPU.h"

void SoundFeedSample(uint16_t left, uint16_t right)
{
	uint16_t frame[2] = {left, right};
	Resampler::Fill(frame, 2);
}

