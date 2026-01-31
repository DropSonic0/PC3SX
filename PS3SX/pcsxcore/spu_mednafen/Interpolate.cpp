#include "stdafx.h"
#include "SPU.h"
#include "Interpolate.h"
#include "Freeze.h"

#include "gauss_i.h"

void			PSX::SPU::Interpolate::Reset				()
{
	memset(Buffer, 0, sizeof(Buffer));
	Parent.SamplePosition = 0x30000;
}

void			PSX::SPU::Interpolate::Stash				(int32_t aSample)
{
	//TODO: Control & 0x4000?
	Buffer[Index ++] = (Parent.ParentSPU().Control & 0x4000) ? CLAMP16(aSample) : 0;
	Index &= 3;
}

int32_t			PSX::SPU::Interpolate::Get					()
{
	int32_t vl = (Parent.SamplePosition >> 6) & ~3;

	int32_t vr = 0;
	for(int i = 0; i != 4; i ++)
	{
		vr += (gauss[vl + i] * Buffer[(Index + i) & 3]) & ~2047;
	}

	return vr >> 11;
}

void			PSX::SPU::Interpolate::Freeze				(Interpolate_State* aState, bool aLoad)
{
	if(aLoad)
	{
		Index = aState->Index;
		for(int i = 0; i < 4; i++)
			Buffer[i] = aState->Buffer[i];
	}
	else
	{
		aState->Index = Index;
		for(int i = 0; i < 4; i++)
			aState->Buffer[i] = Buffer[i];
	}
}
