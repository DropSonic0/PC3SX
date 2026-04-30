#ifndef SPU_INTERPOLATION_H
#define SPU_INTERPOLATION_H

namespace	PSX
{
	namespace	SPU
	{
		class					Channel;

		class					Interpolate
		{
			public:
								Interpolate			(Channel& aParent) : Parent(aParent){}

				void			Reset				();
				void			Stash				(int32_t aSample);
				int32_t			Get					();

			private:
				Channel&		Parent;

				uint32_t		Index;
				uint32_t		Buffer[4];
		};
	}
}

#endif
