//Base includes
#include <mednafen/mednafen.h>
#define MODULENAMESPACE pcsxr
#define SECONDARYINCLUDE
#include <module_helper.h>
using namespace pcsxr;

#ifdef __CELLOS_LV2__ //math.h must NOT be included in an 'extern "C"' block.
#include <math.h>
#endif

extern "C"
{
	#include "stdafx.h"
	#define _IN_OSS
	#include "externals.h"
}

//Implement sound driver
extern "C"
{
	// SETUP SOUND
	void SetupSound(void)
	{
	}

	// REMOVE SOUND
	void RemoveSound(void)
	{
	}

	// GET BYTES BUFFERED
	unsigned long SoundGetBytesBuffered(void)
	{
		return Resampler::Written() * 2;
	}

	// FEED SOUND DATA
	void SoundFeedStreamData(unsigned char* pSound,long lBytes)
	{
		Resampler::Fill(pSound, lBytes / 2);
	}
}
