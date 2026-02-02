#ifndef PSXSPU_DISCSTREAMER_H
#define PSXSPU_DISCSTREAMER_H

namespace	PSX
{
	namespace	SPU
	{
		class							SPU;

		class							DiscStreamer
		{
			public:
										DiscStreamer					(SPU& aParent) :
					Parent(aParent), Feed(0), Play(0), Frequency(44100), Speed(0.0f), Position(0.0f)
				{
					Volume[0] = 0x8000;
					Volume[1] = 0x8000;
				}

				void					SetVolume						(bool aRight, int32_t aValue);

				void					Mix								(int32_t* aLeft, int32_t* aRight);
				void					FeedXA							(xa_decode_t* aData);
				void					FeedCDDA						(int16_t* aSamples, uint32_t aFrames);

			private:
				SPU&					Parent;

				int16_t					Data[65536 * 2];

				uint32_t				Feed;
				uint32_t				Play;

				uint32_t				Frequency;
				double					Speed;
				double					Position;

				int32_t					Volume[2];
		};
	}
}

#endif
