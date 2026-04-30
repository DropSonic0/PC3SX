#ifndef PSXSPU_ADSR_H
#define PSXSPU_ADSR_H

namespace	PSX
{
	namespace	SPU
	{
		class					Channel;

		class					Envelope
		{
			public:
				///Setup internal tables, call once at load
				static void		Initialize				();

								Envelope				(Channel& aParent) :
					Parent(aParent),
					State(0), AttackModeExp(false), AttackRate(0), DecayRate(0), SustainLevel(1024),
					SustainModeExp(false), SustainIncrease(false), SustainRate(0), ReleaseModeExp(false),
					ReleaseRate(0), EnvelopeVol(0), EnvelopeVol_f(0), lVolume(0)
				{
				}

				void			Reset					();
				void			Silence					(bool aImmediate);

				void			WriteRegister			(uint32_t aRegister, uint16_t aValue);
				int32_t			GetVolume				() {return EnvelopeVol;}

				int				Mix						();

				void			DoIncrease				(uint32_t aValue);
				void			DoDecrease				(uint32_t aValue, bool aExponetial);


			private:
				Channel&		Parent;

				int				State;
				bool			AttackModeExp;
				int				AttackRate;

				int				DecayRate;

				int				SustainLevel;
				bool			SustainModeExp;
				bool			SustainIncrease;
				int				SustainRate;

				bool			ReleaseModeExp;
				int				ReleaseRate;

				int				EnvelopeVol;
				int				EnvelopeVol_f;			// fraction
				long			lVolume;
		};
	}
}

#endif

/***************************************************************************
                           adsr.h  -  description
                             -------------------
    begin                : Wed May 15 2002
    copyright            : (C) 2002 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

