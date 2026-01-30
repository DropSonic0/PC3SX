/***************************************************************************
                            spu.c  -  description
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
#include "NoiseGenerator.h"
#include "DiscStreamer.h"
#include "registers.h"

void (CALLBACK *irqCallback)(void)=0;                  // func of main emu, called on spu irq
void (CALLBACK *cddavCallback)(unsigned short,unsigned short)=0;
void				SoundFeedSample		(uint16_t left, uint16_t right);

							PSX::SPU::SPU::SPU				() :
	Streamer(*this), Noise(*this),
	CycleCounter(0), StreamIndex(0), Control(0), Status(0), Address(0), IRQAddress(0), IRQHit(false)
{
	//Initialize ADSR tables
	PSX::SPU::Envelope::Initialize();

	//Create the channels
	for(int i = 0; i != 24; i ++)
	{
		Channels[i] = new Channel(*this);
	}

	//Set channel decode buffers
	Channels[0]->SetDecodeBuffer(true, 0x800);
	Channels[2]->SetDecodeBuffer(true, 0xC00);
}

void						PSX::SPU::SPU::Process			(uint32_t aCycles)
{
	CycleCounter += aCycles;

	const uint32_t CPU_CLOCK = 33868800;
	while(CycleCounter >= CPU_CLOCK / 44100)
	{
		ProduceSample();
		CycleCounter -= CPU_CLOCK / 44100;
	}
}

void						PSX::SPU::SPU::ProduceSample	()
{
	//Tick the noise function
	Noise.Tick();

	//Mix the channels
	int leftSample = 0, rightSample = 0;

	for(int j = 0; j != 24; j ++)
	{
		Channels[j]->Mix(j, &leftSample, &rightSample);
	}

	//XA and CDDA streams
	Streamer.Mix(&leftSample, &rightSample);

	//Feed
	SoundFeedSample(CLAMP16(leftSample), CLAMP16(rightSample));

	//Update the decode position and set the status flag
	StreamIndex = (StreamIndex + 2) & 0x3FF;
	Status &= ~STAT_DECODED;
	Status |= (StreamIndex >= 0x200) ? STAT_DECODED : 0;

	//TODO: Frequency responce?
}

void						PSX::SPU::SPU::WriteRegister	(uint32_t aRegister, uint16_t aValue)
{
	aRegister &= 0xFFF;
	RegisterArea[(aRegister - 0xC00) / 2] = aValue;

	//Channel write
	if(aRegister >= 0xC00 && aRegister < 0xD80)
	{
		Channels[(aRegister >> 4) - 0xC0]->WriteRegister(aRegister & 0xF, aValue);
		return;
	}

	//Others
	switch(aRegister)
	{
		//Channel masks
		case H_Noise1:  for(int i = 0; i != 16; i ++)	Channels[i]->SetNoise(aValue & (1 << i)); break;
		case H_Noise2:  for(int i = 0; i != 8; i ++)	Channels[16 + i]->SetNoise(aValue & (1 << i)); break;
		case H_RVBon1:  for(int i = 0; i != 16; i ++)	Channels[i]->SetReverb(aValue & (1 << i)); break;
		case H_RVBon2:  for(int i = 0; i != 8; i ++)	Channels[16 + i]->SetReverb(aValue & (1 << i)); break;
		case H_FMod1:   for(int i = 0; i != 16; i ++)	Channels[i]->SetFM(aValue & (1 << i)); break;
		case H_FMod2:   for(int i = 0; i != 8; i ++)	Channels[16 + i]->SetFM(aValue & (1 << i)); break;
		case H_SPUon1:  for(int i = 0; i != 16; i ++)	if(aValue & (1 << i)) Channels[i]->TurnOn(); break;
		case H_SPUon2:  for(int i = 0; i != 8; i ++)	if(aValue & (1 << i)) Channels[16 + i]->TurnOn(); break;
		case H_SPUoff1: for(int i = 0; i != 16; i ++)	if(aValue & (1 << i)) Channels[i]->TurnOff(); break;
		case H_SPUoff2: for(int i = 0; i != 8; i ++)	if(aValue & (1 << i)) Channels[16 + i]->TurnOff(); break;

		//Address
		case H_SPUaddr:		Address = aValue * 8; break;
		case H_SPUirqAddr:	IRQAddress = aValue * 8; break;

		//Volume
	    case H_CDLeft: case H_CDRight: Streamer.SetVolume(aRegister == H_CDRight, aValue); break;

    //-------------------------------------------------//
		case H_SPUdata:
		{
			Stash<uint16_t, true>(Address, aValue);

			Address += 2;
			Address &= 0x7FFFF;
		}
		break;
    //-------------------------------------------------//
		case H_SPUctrl:
			Control = aValue;

			// flags
			Status &= ~CTRL_CD_PLAY;
			Status |= (Control & CTRL_CD_PLAY);

			Status &= ~(STAT_CD_REVERB | STAT_EXT_PLAY | STAT_EXT_REVERB | STAT_DMA_NON | STAT_DMA_R | STAT_DMA_W);
			Status |= (Control & CTRL_CD_REVERB) ? STAT_CD_REVERB : 0;
			Status |= (Control & CTRL_EXT_PLAY) ? STAT_EXT_PLAY : 0;
			Status |= (Control & CTRL_EXT_REVERB) ? STAT_EXT_REVERB : 0;
			Status |= (Control & CTRL_DMA_F) ? STAT_DMA_F : 0;
			Status |= ((Control & CTRL_DMA_F) == CTRL_DMA_R) ? STAT_DMA_R : 0;

			if(!(Control & CTRL_IRQ))
			{
				IRQHit = false;
				Status &= ~STAT_IRQ;
			}

			Noise.SetClock((Control & CTRL_NOISE) >> 8);
			break;

		case H_SPUstat: Status = aValue & 0xF800; break;



#if 0
    case H_SPUReverbAddr:

      if(val==0xFFFF || val<=0x200)
       {rvb.StartAddr=rvb.CurrAddr=0;}
      else
       {
        const long iv=(unsigned long)val<<2;
        if(rvb.StartAddr!=iv)
         {
          rvb.StartAddr=(unsigned long)val<<2;
          rvb.CurrAddr=rvb.StartAddr;
         }
       }
      break;
    //-------------------------------------------------//
    //-------------------------------------------------//
    case H_SPUrvolL:
      rvb.VolLeft=val;
      break;
    //-------------------------------------------------//
    case H_SPUrvolR:
      rvb.VolRight=val;
      break;
    //-------------------------------------------------//
    //-------------------------------------------------//

    //-------------------------------------------------//

    //-------------------------------------------------//
    //-------------------------------------------------//
    case H_Reverb+0:    rvb.FB_SRC_A=val; break;
    case H_Reverb+2   : rvb.FB_SRC_B=(short)val;       break;
    case H_Reverb+4   : rvb.IIR_ALPHA=(short)val;      break;
    case H_Reverb+6   : rvb.ACC_COEF_A=(short)val;     break;
    case H_Reverb+8   : rvb.ACC_COEF_B=(short)val;     break;
    case H_Reverb+10  : rvb.ACC_COEF_C=(short)val;     break;
    case H_Reverb+12  : rvb.ACC_COEF_D=(short)val;     break;
    case H_Reverb+14  : rvb.IIR_COEF=(short)val;       break;
    case H_Reverb+16  : rvb.FB_ALPHA=(short)val;       break;
    case H_Reverb+18  : rvb.FB_X=(short)val;           break;
    case H_Reverb+20  : rvb.IIR_DEST_A0=(short)val;    break;
    case H_Reverb+22  : rvb.IIR_DEST_A1=(short)val;    break;
    case H_Reverb+24  : rvb.ACC_SRC_A0=(short)val;     break;
    case H_Reverb+26  : rvb.ACC_SRC_A1=(short)val;     break;
    case H_Reverb+28  : rvb.ACC_SRC_B0=(short)val;     break;
    case H_Reverb+30  : rvb.ACC_SRC_B1=(short)val;     break;
    case H_Reverb+32  : rvb.IIR_SRC_A0=(short)val;     break;
    case H_Reverb+34  : rvb.IIR_SRC_A1=(short)val;     break;
    case H_Reverb+36  : rvb.IIR_DEST_B0=(short)val;    break;
    case H_Reverb+38  : rvb.IIR_DEST_B1=(short)val;    break;
    case H_Reverb+40  : rvb.ACC_SRC_C0=(short)val;     break;
    case H_Reverb+42  : rvb.ACC_SRC_C1=(short)val;     break;
    case H_Reverb+44  : rvb.ACC_SRC_D0=(short)val;     break;
    case H_Reverb+46  : rvb.ACC_SRC_D1=(short)val;     break;
    case H_Reverb+48  : rvb.IIR_SRC_B1=(short)val;     break;
    case H_Reverb+50  : rvb.IIR_SRC_B0=(short)val;     break;
    case H_Reverb+52  : rvb.MIX_DEST_A0=(short)val;    break;
    case H_Reverb+54  : rvb.MIX_DEST_A1=(short)val;    break;
    case H_Reverb+56  : rvb.MIX_DEST_B0=(short)val;    break;
    case H_Reverb+58  : rvb.MIX_DEST_B1=(short)val;    break;
    case H_Reverb+60  : rvb.IN_COEF_L=(short)val;      break;
    case H_Reverb+62  : rvb.IN_COEF_R=(short)val;      break;
#endif
   }
}

int16_t					PSX::SPU::SPU::ReadRegister			(uint32_t aRegister)
{
	aRegister &= 0xFFF;

	//Channel Read
	if(aRegister >= 0xC00 && aRegister < 0xD80)
	{
		if((aRegister & 0xF) == 12)
		{
			return Channels[(aRegister >> 4) - 0xC0]->ADSR.GetVolume();
		}

		//TODO: Some games read other registers. eg Final Fantasy VII will read 8 and 10 (the envelope registers).
		//TODO: ^Do the values of these change over time?
	}

	switch(aRegister)
	{
		case H_SPUctrl:	return Control;
		case H_SPUstat: return Status;

		case H_SPUdata:
		{
			uint16_t result = Fetch<uint16_t, false>(Address);
			Address = (Address + 2) & 0x7FFFF;
			return result;
		}
	}

	return RegisterArea[(aRegister - 0xC00) / 2];
}

void						PSX::SPU::SPU::DMAWrite			(uint16_t* aSource, int32_t aSize)
{
	assert(aSource && aSize > 0);

	Status |= STAT_DATA_BUSY;

	for(int i = 0; i != aSize; i ++)
	{
		Stash<uint16_t, true>(Address, aSource[i]);

		Address += 2;

		if(Address >= 0x80000) // Vib Ribbon - stop transfer (reverb playback)
		{
			break;
		}
	}

	Status &= ~(STAT_DATA_BUSY | STAT_DMA_NON | STAT_DMA_R);
	Status |= STAT_DMA_W;
}

void						PSX::SPU::SPU::DMARead			(uint16_t* aTarget, int32_t aSize)
{
	assert(aTarget && aSize > 0);

	Status |= STAT_DATA_BUSY;

	for(int i = 0; i != aSize; i ++)
	{
		CheckIRQ(Address, 0);
		aTarget[i] = Fetch<uint16_t, false>(Address);
		Address += 2;

		if(Address >= 0x80000) // guess based on Vib Ribbon (above)
		{
			break;
		}
	}

	Status &= ~(STAT_DATA_BUSY | STAT_DMA_NON | STAT_DMA_W);
	Status |= STAT_DMA_R;
}

bool					PSX::SPU::SPU::CheckIRQ			(uint32_t aAddress, bool aForce)
{
	if(Control & CTRL_IRQ)
	{
		if((!IRQHit) && (aForce || IRQAddress == aAddress))
		{
			if(irqCallback) irqCallback();

			IRQHit = true;
			Status |= STAT_IRQ;
			return true;
		}
	}

	return false;
}

