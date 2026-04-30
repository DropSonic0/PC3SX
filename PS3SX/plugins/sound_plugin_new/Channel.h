#ifndef PSXSPU_CHANNEL_H
#define PSXSPU_CHANNEL_H

#include "Envelope.h"
#include "Interpolate.h"

namespace	PSX
{
	namespace	SPU
	{
		class						Envelope;
		class						Channel;
		class						SPU;

		class						SampleStream
		{
			public:
									SampleStream			(Channel& aParent) : 
					Parent(aParent), Start(0), Current(0), Loop(0), Index(28)
				{
				}


				void				WriteRegister			(uint32_t aRegister, uint16_t aValue);

				void				Reset					();

				int32_t				Fetch					();
				void				NextBlock				();
				uint32_t			DecodeBlock				();

				uint32_t			GetCurrent				() {return Current;};
				uint32_t			GetLoop					() {return Loop;}

			private:
				Channel&			Parent;								///Channel

				uint32_t			Start;								///Sample start address.
				uint32_t			Current;							///Current play address.
				uint32_t			Loop;								///Loop target address.

				int32_t				DecodeBuffer[28];					///Buffer of currently decoded sample block.
				uint32_t			Index;								///Current sample in the decode buffer.

				int32_t				PreviousSamples[2];					///Two last decoded samples, for ADPCM decode filter.
		};

		class						Channel
		{
			public:
									Channel					(SPU& aParent) :
					Parent(aParent), ADSR(*this), Sample(*this), Filter(*this),
					DecodeStream(0), HasDecodeBuffer(false), SamplePosition(0), SampleIncrement(0),
					bReverb(0), bRVBActive(0), bNoise(0), bFMod(0), RawPitch(0)
				{

				}

				SPU&				ParentSPU				(){return Parent;}

				void				SetDecodeBuffer			(bool aOn, uint32_t aStream);

				void				WriteRegister			(int aRegister, uint16_t aValue);

				void				TurnOn					();
				void				TurnOff					();

				void				Start					(int ch);
				void				Mix						(int ch, int32_t* aLeftSum, int32_t* aRightSum);
				void				UpdateFrequency			();

				void				SetReverb				(bool aOn) {bReverb = aOn;}
				void				SetNoise				(bool aOn) {bNoise = aOn;}
				void				SetFM					(bool aOn) {bFMod = aOn;}

			public:
				//Decode buffers
				uint32_t			DecodeStream;
				bool				HasDecodeBuffer;

				//

				int					SamplePosition;			//16:16 Fixed point
				int					SampleIncrement;		//16:16 Fixed point

				int					bReverb;                            // can we do reverb on this channel? must have ctrl register bit, to get active
				int					bRVBActive;                         // reverb active flag
				int					bNoise;                             // noise active flag
				int					bFMod;                              // freq mod (0=off, 1=sound channel, 2=freq channel)

				uint32_t			RawPitch;				//Pitch value

				SPU&				Parent;
				Envelope			ADSR;
				SampleStream		Sample;
				Interpolate			Filter;

			private:
				int32_t				Volume[2];
		};
	}
}

#endif

