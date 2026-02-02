#ifndef PSXSPU_SPU_H
#define	PSXSPU_SPU_H

#include <vector>

#include "NoiseGenerator.h"
#include "Channel.h"
#include "DiscStreamer.h"

namespace	PSX
{
	namespace	SPU
	{
		class							SPU
		{
			public:
										SPU					();

				void					Process				(uint32_t aCycles);
				void					ProduceSample		();

				void					WriteRegister		(uint32_t aRegister, uint16_t aValue);
				int16_t					ReadRegister		(uint32_t aRegister);

				void					DMAWrite			(uint16_t* aSource, int32_t aSize);
				void					DMARead				(uint16_t* aTarget, int32_t aSize);

				bool					CheckIRQ			(uint32_t aAddress, bool aForce);

				void					StoreDecoded		(uint32_t aStream, int16_t aValue)
				{
					assert(aStream < 4 && StreamIndex < 0x400);
					Stash<int16_t, true>((aStream * 0x400) + StreamIndex, aValue);
				}

				template <typename T, bool tIRQ>
				void					Stash				(uint32_t aAddress, T aValue)
				{
					assert(sizeof(T) == 1 || sizeof(T) == 2);
					assert(aAddress < 512 * 1024 && !(aAddress & (sizeof(T) - 1)));

					if(tIRQ)
					{
						CheckIRQ(aAddress, 0);
					}

					T* memory = (T*)Memory;
					memory[aAddress / sizeof(T)] = aValue;
				}

				template <typename T, bool tIRQ>
				T						Fetch				(uint32_t aAddress)
				{
					assert(sizeof(T) == 1 || sizeof(T) == 2);
					assert(aAddress < 512 * 1024 && !(aAddress & (sizeof(T) - 1)));

					if(tIRQ)
					{
						CheckIRQ(aAddress, 0);
					}

					T* memory = (T*)Memory;
					return memory[aAddress / sizeof(T)];
				}

			public: //HACK
				Channel*				Channels[24];
				DiscStreamer			Streamer;
				NoiseGenerator			Noise;

				uint16_t				Control;
				uint16_t				Status;

			private:
				uint32_t				CycleCounter;
				uint32_t				StreamIndex;

				uint16_t				RegisterArea[0x10000];
				uint8_t					Memory[512 * 1024];

				uint32_t				Address;

				uint16_t				IRQAddress;
				bool					IRQHit;
		};
	}
}

extern void (CALLBACK *cddavCallback)(unsigned short,unsigned short);
extern void (CALLBACK *irqCallback)(void);                  // func of main emu, called on spu irq

#endif
