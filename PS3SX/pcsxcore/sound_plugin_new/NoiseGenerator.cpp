#include <stdint.h>
#include "stdafx.h"
#include "SPU.h"
#include "NoiseGenerator.h"

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

	};
};
