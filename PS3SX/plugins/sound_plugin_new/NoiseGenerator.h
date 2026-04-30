#ifndef SPU_NOISE_H
#define SPU_NOISE_H

namespace	PSX
{
	namespace	SPU
	{
		class				SPU;

		class				NoiseGenerator
		{
			public:
							NoiseGenerator				(SPU& aParent) : Parent(aParent), Value(1), Count(0), Clock(0)	{}

				void		SetClock					(uint32_t aClock)					{assert(aClock < 0x40); Clock = aClock;}
				int32_t		GetValue					()									{return Value;}

				//Call once every sample
				void		Tick						()
				{
					uint32_t level = (0x8000 >> (Clock >> 2)) << 16;

					Count += 0x10000 + NoiseFreqAdd[Clock & 3];
					if((Count & 0xFFFF) >= NoiseFreqAdd[4])
					{
						Count += 0x10000;
						Count -= NoiseFreqAdd[Clock & 3];
					}

					assert(level);
					if(Count >= level)
					{
						Count %= level;
						Value = (Value << 1) | NoiseWaveAdd[(Value >> 10) & 63];
					}
				}


			private:
				SPU&		Parent;

				int16_t		Value;
				uint32_t	Count;
				uint32_t	Clock;

				static const int8_t		NoiseWaveAdd[64];
				static const uint16_t	NoiseFreqAdd[5];
		};
	};
};

/*
Noise Algorithm
- Dr.Hell (Xebra PS1 emu)
- 100% accurate (waveform + frequency)
- http://drhell.web.fc2.com


Level change cycle
Freq = 0x8000 >> (NoiseClock >> 2);

Frequency of half cycle
Half = ((NoiseClock & 3) * 2) / (4 + (NoiseClock & 3));
- 0 = (0*2)/(4+0) = 0/4
- 1 = (1*2)/(4+1) = 2/5
- 2 = (2*2)/(4+2) = 4/6
- 3 = (3*2)/(4+3) = 6/7

-------------------------------

5*6*7 = 210
4 -  0*0 = 0
5 - 42*2 = 84
6 - 35*4 = 140
7 - 30*6 = 180
*/

#endif
