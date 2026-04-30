#include "stdafx.h"
#include "registers.h"
#include "SPU.h"
#include "Channel.h"
#include "Interpolate.h"
#include "NoiseGenerator.h"

//HACK
static int32_t FMValue;
//ROBO: What the fuck is 'f' supposed to be????
static const int f[5][2] = {{0,  0}, {60, 0}, {115, -52}, {98, -55}, {122, -60}};

void				PSX::SPU::SampleStream::WriteRegister		(uint32_t aRegister, uint16_t aValue)
{
	//Brain Dead 13 - align to 16 byte boundary
	if(aRegister == 6)
	{
		Start = (uint32_t)((aValue << 3) & ~0xF);
	}
	else if(aRegister == 14)
	{
		Loop = (uint32_t)((aValue << 3) & ~0xF);
	}
	else
	{
		assert(false);
	}
}

void				PSX::SPU::SampleStream::Reset				()
{
	Current = Start;

	PreviousSamples[0] = 0;
	PreviousSamples[1] = 0;
	Index = 28;
}

int32_t				PSX::SPU::SampleStream::Fetch				()
{
	if(Index == 28)
	{
		NextBlock();
	}

	return DecodeBuffer[Index ++];
}

void				PSX::SPU::SampleStream::NextBlock			()
{
	//Reset the play position
	Index = 0;

	//Decode the sample
	uint32_t flags = DecodeBlock();
	Current += 16;

	//Check the address of the middle of the last, and the start of the new, samples for IRQ hits
	//TODO: Timing Issue? Should the first IRQ trigger exactly when Current - 8 would have been accessed?
	Parent.ParentSPU().CheckIRQ(Current - 8, false);
	Parent.ParentSPU().CheckIRQ(Current - 0, false);

	//Set the loop target
	if(flags & 4)
	{
		Loop = Current - 16;
	}

	//Jump to the loop point
	if(flags & 1)
	{
		Current = Loop;

		//Finish the sample
		//TODO: This should silence immediately?
		if(!(flags & 2))
		{
			Parent.ADSR.Silence(true);
		}
	}

	//Force playback to not pass the end of SPU memory (Silhouette Mirage - ending mini-game?)
	if(Current >= 0x80000)
	{
		Current -= 16;
		Parent.ADSR.Silence(true);
	}
}

uint32_t			PSX::SPU::SampleStream::DecodeBlock			()
{
	//Read the sample header
	uint32_t predict_nr = ((Parent.ParentSPU().Fetch<uint8_t, false>(Current) >> 4) <= 4) ? Parent.ParentSPU().Fetch<uint8_t, false>(Current) >> 4 : 0;				//Silhouette Mirage - Serah fight: > 4 = 0
	uint32_t shift_factor = Parent.ParentSPU().Fetch<uint8_t, false>(Current) & 0xF;
	uint32_t flags = Parent.ParentSPU().Fetch<uint8_t, false>(Current + 1);

	//Decode sample
	for(int i = 0; i != 14; i ++)
	{
		//Get a byte from the sample
		int32_t sampleByte = Parent.ParentSPU().Fetch<uint8_t, false>(Current + 2 + i);

		//Decode both halves
		for(int j = 0; j != 2; j ++)
		{
			int32_t s = (sampleByte & (j ? 0xF0 : 0xF)) << (j ? 8 : 12);
			s |= (s & 0x8000) ? 0xFFFF0000 : 0;

			int32_t decodedSample = s >> shift_factor;
			decodedSample += ((PreviousSamples[0] * f[predict_nr][0]) >> 6) + ((PreviousSamples[1] * f[predict_nr][1]) >> 6);
			decodedSample = CLAMP16(decodedSample);

			PreviousSamples[1] = PreviousSamples[0];
			PreviousSamples[0] = DecodeBuffer[i * 2 + j] = decodedSample;
		}
	}

	//Return the flags
	return flags;
}

////

void				PSX::SPU::Channel::SetDecodeBuffer			(bool aOn, uint32_t aStream)
{
	DecodeStream = aStream;
	HasDecodeBuffer = aOn;
}


void				PSX::SPU::Channel::WriteRegister			(int aRegister, uint16_t aValue)
{
	//Volume
	if(aRegister == 0 || aRegister == 2)
	{
		if(aValue & 0x8000)
		{
			//TODO: Sweeps not supported, just mute
			Volume[(aRegister == 2) ? 1 : 0] = 0;
		}
		else
		{
			//Mask the volume bits from the value
			uint16_t volume = aValue & 0x3FFF;

			//Handle phase invert
			//TODO: This may be wrong? A test case is Final Fantasy VII, many of the battle sound effects use it to process noise values.
			Volume[(aRegister == 2) ? 1 : 0] = (aValue & 0x4000) ? 0x3FFF - volume : volume;
		}
	}
	//Pitch
	else if(aRegister == 4)
	{
		//TODO: Mask or clamp?
		RawPitch = aValue & 0x3FFF;

		SampleIncrement = RawPitch << 4;
		SampleIncrement = SampleIncrement ? SampleIncrement : 1;
	}
	//Sample stream
	else if(aRegister == 6 || aRegister == 14)
	{
		Sample.WriteRegister(aRegister, aValue);
	}
	//Envelope
	else if(aRegister >= 8 && aRegister <= 12)
	{
		ADSR.WriteRegister(aRegister, aValue);
	}
	else
	{
		assert(false);
	}
}

void				PSX::SPU::Channel::TurnOn					()
{
	ADSR.Reset();
	Filter.Reset();
	Sample.Reset();
}

void				PSX::SPU::Channel::TurnOff					()
{
	ADSR.Silence(false);
}

void				PSX::SPU::Channel::Mix						(int ch, int32_t* aLeftSum, int32_t* aRightSum)
{
	//Update frequency
	UpdateFrequency();

	//Update the sample
	while(SamplePosition >= 0x10000L)
	{
		//Add next sample to interpolation buffer
		Filter.Stash(Sample.Fetch());
		SamplePosition -= 0x10000L;
	}

	//Apply envelope
	int16_t sampleValue = CLAMP16((ADSR.Mix() * (bNoise ? Parent.Noise.GetValue() : Filter.Get())) / 1023);

	//Decoded data buffer
	if(HasDecodeBuffer)
	{
		Parent.Stash<int16_t, true>(DecodeStream, sampleValue);
	}

	//Store the value for frequency modulation
	FMValue = sampleValue;

	//Mix the channel into the buffer
	*aLeftSum += (sampleValue * Volume[0]) / 0x4000L;
	*aRightSum += (sampleValue * Volume[1]) / 0x4000L;

	//! Something to do with interpolation?
	SamplePosition += SampleIncrement;
}


void				PSX::SPU::Channel::UpdateFrequency			()
{
	if(bFMod == 1)
	{
		int newPitch = ((32768L + FMValue) * RawPitch) / 32768L;
		if(newPitch > 0x3fff) newPitch = 0x3fff;
		if(newPitch < 0x1)    newPitch = 0x1;

		newPitch = (44100L * newPitch) / 4096L;

		SampleIncrement = ((newPitch / 10) << 16) / 4410;
		SampleIncrement = SampleIncrement ? SampleIncrement : 1;
	}

	FMValue = 0;
}

