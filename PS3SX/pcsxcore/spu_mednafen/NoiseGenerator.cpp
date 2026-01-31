#include <stdint.h>
#include "stdafx.h"
#include "SPU.h"
#include "NoiseGenerator.h"
#include "Freeze.h"

namespace	PSX
{
	namespace	SPU
	{
		const int8_t					NoiseGenerator::NoiseWaveAdd[64] =
		{
			1, 0, 0, 1, 0, 1, 1, 0,
			1, 0, 0, 1, 0, 1, 1, 0,
			1, 0, 0, 1, 0, 1, 1, 0,
			1, 0, 0, 1, 0, 1, 1, 0,
			0, 1, 1, 0, 1, 0, 0, 1,
			0, 1, 1, 0, 1, 0, 0, 1,
			0, 1, 1, 0, 1, 0, 0, 1,
			0, 1, 1, 0, 1, 0, 0, 1
		};

		const uint16_t					NoiseGenerator::NoiseFreqAdd[5] =
		{
			0, 84, 140, 180, 210
		};

		void						NoiseGenerator::Freeze				(NoiseGenerator_State* aState, bool aLoad)
		{
			if(aLoad)
			{
				Value = aState->Value;
				Count = aState->Count;
				Clock = aState->Clock;
			}
			else
			{
				aState->Value = Value;
				aState->Count = Count;
				aState->Clock = Clock;
			}
		}

	};
};
