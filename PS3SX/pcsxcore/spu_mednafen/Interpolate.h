#ifndef SPU_INTERPOLATION_H
#define SPU_INTERPOLATION_H

namespace	PSX
{
	namespace	SPU
	{
		class					Channel;
		struct Interpolate_State;

		class					Interpolate
		{
			public:
								Interpolate			(Channel& aParent) : Parent(aParent){}

				void			Reset				();
				void			Stash				(int32_t aSample);
				int32_t			Get					();

				void			Freeze				(Interpolate_State* aState, bool aLoad);

			private:
				Channel&		Parent;

				uint32_t		Index;
				uint32_t		Buffer[4];
		};
	}
}

#endif
