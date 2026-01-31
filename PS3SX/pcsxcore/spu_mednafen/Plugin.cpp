#include "stdafx.h"
#include "plugins.h"
#include "SPU.h"
#include "Envelope.h"
#include "Freeze.h"

static PSX::SPU::SPU psxspu;


extern "C"
{
	unsigned short CALLBACK				SPUreadDMA				(void)															{assert(false); return 0;}
	void CALLBACK						SPUreadDMAMem			(unsigned short * pusPSXMem,int iSize)							{psxspu.DMARead(pusPSXMem, iSize);}
	void CALLBACK						SPUwriteDMA				(unsigned short val)											{assert(false);}
	void CALLBACK						SPUwriteDMAMem			(unsigned short * pusPSXMem,int iSize)							{psxspu.DMAWrite(pusPSXMem, iSize);}
	void CALLBACK						SPUregisterCallback		(void (CALLBACK *callback)(void))								{irqCallback = callback;}
	void CALLBACK						SPUregisterCDDAVolume	(void (CALLBACK *CDDAVcallback)(unsigned short,unsigned short))	{cddavCallback = CDDAVcallback;}
	long CALLBACK						SPUtest					(void)															{return 0;}
	long CALLBACK						SPUconfigure			(void)															{return 0;}
	void CALLBACK						SPUabout				(void)															{}
	void CALLBACK						SPUupdate				(void)															{assert(false);}
	long CALLBACK						SPUinit					(void)															{return 0;}
	long								SPUopen					(void)															{return 0;}
	long CALLBACK						SPUclose				(void)															{return 0;}
	long CALLBACK						SPUshutdown				(void)															{return 0;}
	void CALLBACK						SPUplayADPCMchannel		(xa_decode_t *xap)												{psxspu.Streamer.FeedXA(xap);}
	void CALLBACK						SPUplayCDDAchannel		(short *pcm, int nbytes)										{psxspu.Streamer.FeedCDDA(pcm, nbytes);}
	void CALLBACK						SPUwriteRegister		(unsigned long reg, unsigned short val)							{psxspu.WriteRegister(reg, val);}
	unsigned short CALLBACK				SPUreadRegister			(unsigned long reg)												{return psxspu.ReadRegister(reg);}
	void CALLBACK						SPUasync				(unsigned int aCycles)											{psxspu.Process(aCycles);}
	
	long CALLBACK						SPUfreeze				(uint32_t ulFreezeMode,SPUFreeze_t * pF)
	{
		if(!pF) return 0;

		if(ulFreezeMode == 2) // Info
		{
			pF->Size = sizeof(SPUFreeze_t) + sizeof(PSX::SPU::SPU_State);
			return 1;
		}

		PSX::SPU::SPU_State* state = (PSX::SPU::SPU_State*)pF->SPUInfo;
		if(!state) // Some versions of PCSX might not handle SPUInfo, so we append it
			state = (PSX::SPU::SPU_State*)(pF + 1);

		if(ulFreezeMode == 1) // Save
		{
			memset(pF, 0, sizeof(SPUFreeze_t));
			strcpy((char*)pF->PluginName, "MDFNSPU");
			pF->Size = sizeof(SPUFreeze_t) + sizeof(PSX::SPU::SPU_State);
			
			// Fill basic fields for compatibility if someone else reads it
			// SPU RAM
			// SPU ports (registers)
			// But since we use our own core, we rely on the appended state.

			psxspu.Freeze(state, false);
			return 1;
		}

		if(ulFreezeMode == 0) // Load
		{
			psxspu.Freeze(state, true);
			return 1;
		}

		return -1;
	}
}
