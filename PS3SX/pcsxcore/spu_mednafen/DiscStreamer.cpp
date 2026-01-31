/***************************************************************************
                            xa.c  -  description
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
#include "registers.h"
#include "DiscStreamer.h"
#include "Freeze.h"

void					PSX::SPU::DiscStreamer::SetVolume						(bool aRight, int32_t aValue)
{
	Volume[aRight ? 1 : 0] = aValue;

	//Tell the emulator
	if(cddavCallback)
	{
		cddavCallback(aRight, aValue);
	}
}

void					PSX::SPU::DiscStreamer::Mix								(int32_t* aLeft, int32_t* aRight)
{
	//TODO: At 37800hz the buffer seems to consistently underrun by around 20 frames with occasional jumps to 40 or even 60;
	//TODO: Either there is something wrong with this code or timing in libpcsxcore needs to be tightened (probably a lot of both)

	int16_t leftChannel = 0, rightChannel = 0;

	//Update frequency adjust position
	Position += Speed;

	//Fetch a frame from the buffer
	while(Position >= 1.0f)
	{
		if(Play != Feed)
		{
			leftChannel = Data[Play * 2];
			rightChannel = Data[Play * 2 + 1];
			Play = (Play + 1) & 0xFFFF;
		}

		Position -= 1.0f;
	}

	//Store samples into SPU memory
	Parent.StoreDecoded(0, leftChannel);
	Parent.StoreDecoded(1, rightChannel);

	//Apply volume
	leftChannel = CLAMP16((leftChannel * Volume[0]) / 0x8000);
	rightChannel = CLAMP16((rightChannel * Volume[1]) / 0x8000);

	//Reverb
	//TODO: Is this ever used?
	if(Parent.Control & CTRL_CD_REVERB)
	{
//		StoreREVERB_CD(leftChannel, rightChannel, 0);
	}

	//Mix
	if(Parent.Control & CTRL_CD_PLAY)
	{
		*aLeft += leftChannel;
		*aRight += rightChannel;
	}
}

void					PSX::SPU::DiscStreamer::FeedXA							(xa_decode_t* aData)
{
	assert(aData && aData->freq);

	//Copy frequency
	Frequency = aData->freq;
	Speed = ((float)Frequency) / 44100.0f;

	//Copy samples
	//TODO: Is nsamples in individual samples or frames?
	for(int i = 0; i != aData->nsamples; i ++)
	{
		Data[Feed * 2 + 0] = aData->pcm[aData->stereo ? (i * 2 + 0) : i];
		Data[Feed * 2 + 1] = aData->pcm[aData->stereo ? (i * 2 + 1) : i];
		Feed = (Feed + 1) & 0xFFFF;

		//Handle overrun...
		if(Feed == Play)
		{
			Feed = Play ? Play - 1 : Feed;
			break;
		}
	}
}

void					PSX::SPU::DiscStreamer::FeedCDDA						(int16_t* aSamples, uint32_t aFrames)
{
	//TODO
}

void					PSX::SPU::DiscStreamer::Freeze							(DiscStreamer_State* aState, bool aLoad)
{
	if(aLoad)
	{
		Feed = aState->Feed;
		Play = aState->Play;
		Frequency = aState->Frequency;
		Speed = aState->Speed;
		Position = aState->Position;
		Volume[0] = aState->Volume[0];
		Volume[1] = aState->Volume[1];
		memcpy(Data, aState->Data, sizeof(Data));
	}
	else
	{
		aState->Feed = Feed;
		aState->Play = Play;
		aState->Frequency = Frequency;
		aState->Speed = Speed;
		aState->Position = Position;
		aState->Volume[0] = Volume[0];
		aState->Volume[1] = Volume[1];
		memcpy(aState->Data, Data, sizeof(Data));
	}
}
