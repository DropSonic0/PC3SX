/***************************************************************************
                          adsr.c  -  description
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

#include "stdafx.h"
#include "SPU.h"
#include "Envelope.h"
#include "Channel.h"

static int					RateTableAdd[128];
static int					RateTableAdd_f[128];
static int					RateTableSub[128];
static int					RateTableSub_f[128];
static const int			RateTable_denom = 1 << (( (4*32)>>2) - 11);

void						PSX::SPU::Envelope::Initialize				()
{
 int lcv;

 memset(RateTableAdd,0,sizeof(int)*128);
 memset(RateTableAdd_f,0,sizeof(int)*128);
 memset(RateTableSub,0,sizeof(int)*128);
 memset(RateTableSub_f,0,sizeof(int)*128);


 // Optimize table - Dr. Hell ADSR math
 for( lcv=0; lcv<48; lcv++ ) {
	 RateTableAdd[lcv] = (7 - (lcv&3)) << (11 - (lcv >> 2));
	 RateTableSub[lcv] = (-8 + (lcv&3)) << (11 - (lcv >> 2));

	 RateTableAdd_f[lcv] = 0;
	 RateTableSub_f[lcv] = 0;
 }

 for( lcv=48; lcv<128; lcv++ ) {
	 int denom;

	 denom = 1 << ((lcv>>2) - 11);

	 // whole
	 RateTableAdd[lcv] = (7 - (lcv&3)) / denom;
	 RateTableSub[lcv] = (-8 + (lcv&3)) / denom;

	 // fraction
	 RateTableAdd_f[lcv] = (7 - (lcv&3)) % denom;
	 RateTableSub_f[lcv] = (-8 + (lcv&3)) % denom;

	 RateTableAdd_f[lcv] *= RateTable_denom / denom;
	 RateTableSub_f[lcv] *= RateTable_denom / denom;

	 // goofy compiler - mod
	 if( RateTableSub_f[lcv] > 0 ) RateTableSub_f[lcv] = -RateTableSub_f[lcv];
 }
}

void						PSX::SPU::Envelope::Reset					()
{
	lVolume = 1;
	State = 0;
	EnvelopeVol = 0;
	EnvelopeVol_f = 0;
}

void						PSX::SPU::Envelope::Silence					(bool aImmediate)
{
	if(aImmediate)
	{
		lVolume = 0;
		EnvelopeVol = 0;
		State = 4;
	}
	else
	{
		State = 3;
	}
}

void						PSX::SPU::Envelope::WriteRegister			(uint32_t aRegister, uint16_t aValue)
{
	if(aRegister == 8)
	{
		AttackModeExp = (aValue & 0x8000);
		AttackRate = (aValue >> 8) & 0x7F;
		DecayRate = (aValue >> 4) & 0xF;
		SustainLevel = aValue & 0xF;
	}
	else if(aRegister == 10)
	{
		SustainModeExp = (aValue & 0x8000);
		SustainIncrease = !(aValue & 0x4000);
		SustainRate = (aValue >> 6) & 0x7F;
		ReleaseModeExp = (aValue & 0x20);
		ReleaseRate = aValue & 0x1F;
	}
}

int							PSX::SPU::Envelope::Mix						()
{
	//Attack
	if(State == 0)
	{
		DoIncrease(AttackRate + (AttackModeExp && (EnvelopeVol >= 0x6000) ? 8 : 0));
		State = (EnvelopeVol == 0x7FFF) ? 1 : 0;
	}
	//Decay
	else if(State == 1)
	{
		DoDecrease(DecayRate * 4, true);
		State = (((EnvelopeVol >> 11) & 0xF) <= SustainLevel) ? 2 : 1;
	}
	//Sustain
	else if(State == 2)
	{
		if(SustainIncrease) DoIncrease(SustainRate + (SustainModeExp && (EnvelopeVol >= 0x6000) ? 8 : 0));
		else				DoDecrease(SustainRate, SustainModeExp);
	}
	//Release
	else if(State == 3)
	{
		DoDecrease(ReleaseRate * 4, ReleaseModeExp);
		State = (EnvelopeVol == 0) ? 4 : 3;
	}
	else
	{
		return 0;
	}

	lVolume = EnvelopeVol >> 5;
	return lVolume;
}

void						PSX::SPU::Envelope::DoIncrease				(uint32_t aValue)
{
	EnvelopeVol += RateTableAdd[aValue];
	EnvelopeVol_f += RateTableAdd_f[aValue];

	//Adjust fractional
	if(EnvelopeVol_f >= RateTable_denom)
	{
		EnvelopeVol_f -= RateTable_denom;
		EnvelopeVol ++;
	}

	//Clamp
	if(EnvelopeVol >= 0x8000)
	{
		EnvelopeVol = 0x7FFF;
		EnvelopeVol_f = RateTable_denom;
	}
}

void						PSX::SPU::Envelope::DoDecrease				(uint32_t aValue, bool aExponetial)
{
	EnvelopeVol += (aExponetial) ? (RateTableSub[aValue] * EnvelopeVol) >> 15 : RateTableSub[aValue];
	EnvelopeVol_f += RateTableSub_f[aValue];

	//Adjust fractional
	if(EnvelopeVol_f < 0)
	{
		EnvelopeVol_f += RateTable_denom;
		EnvelopeVol --;
	}

	//Clamp
	if(EnvelopeVol < 0)
	{
		EnvelopeVol = 0;
		EnvelopeVol_f = 0;
	}
}

/*
ADSR
- Dr. Hell (Xebra PS1 emu)
- Accurate (!)
- http://drhell.web.fc2.com


Envelope increase
0-47: (7 - (RATE & 3)) <<(11 - (RATE>> 2))
48+:  7 - (RATE & 3) / (1 <<((RATE>> 2) - 11))

Envelope decrease
0-47: (-8 + (RATE & 3)) <<(11 - (RATE>> 2))
48+:  -8 + (RATE & 3) / (1 <<((RATE>> 2) - 11))


Exponential increase
0000-5FFF = (rate + 0)
6000+     = (rate + 8)

Exponential decrease
(molecules (decrease) * level)>> 15

-----------------------------------

Fraction (release rate)
1<<((4*32>>2)-11) = 1<<21


Increase
40 = (7-0)<<(11-10) = 7<<1 = 14
41 = (7-1)<<(11-10) = 6<<1 = 12
42 = (7-2)<<(11-10) = 5<<1 = 10
43 = (7-3)<<(11-10) = 4<<1 = 8

44 = (7-0)<<(11-11) = 7<<0 = 7
45 = (7-1)<<(11-11) = 6<<0 = 6
46 = (7-2)<<(11-11) = 5<<0 = 5
47 = (7-3)<<(11-11) = 4<<0 = 4
--
48 = (7-0) / 1<<(12-11) = 7 / 2
49 = (7-1) / 1<<(12-11) = 6 / 2
50 = (7-2) / 1<<(12-11) = 5 / 2
51 = (7-3) / 1<<(12-11) = 4 / 2

52 = (7-0) / 1<<(13-11) = 7 / 4
56 = (7-0) / 1<<(14-11) = 7 / 8
60 = (7-0) / 1<<(15-11) = 7 / 16


Decrease
40 = (-8+0)<<(11-10) = -8<<1 = -16
41 = (-8+1)<<(11-10) = -7<<1 = -14
42 = (-8+2)<<(11-10) = -6<<1 = -12
43 = (-8+3)<<(11-10) = -5<<1 = -10

44 = (-8+0)<<(11-11) = -8<<0 = -8
45 = (-8+1)<<(11-11) = -7<<0 = -7
46 = (-8+2)<<(11-11) = -6<<0 = -6
47 = (-8+3)<<(11-11) = -5<<0 = -5
--
48 = (-8+0) / 1<<(12-11) = -8 / 2
49 = (-8+1) / 1<<(12-11) = -7 / 2
50 = (-8+2) / 1<<(12-11) = -6 / 2
51 = (-8+3) / 1<<(12-11) = -5 / 2
*/
