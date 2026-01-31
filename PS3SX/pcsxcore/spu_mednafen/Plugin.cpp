#include "stdafx.h"
#include "plugins.h"
#include "SPU.h"
#include "Envelope.h"
#include "Freeze.h"

static PSX::SPU::SPU psxspu;


extern "C"
{
	unsigned short CALLBACK				MDFNSPUreadDMA				(void)															{assert(false); return 0;}
	void CALLBACK						MDFNSPUreadDMAMem			(unsigned short * pusPSXMem,int iSize)							{psxspu.DMARead(pusPSXMem, iSize);}
	void CALLBACK						MDFNSPUwriteDMA				(unsigned short val)											{assert(false);}
	void CALLBACK						MDFNSPUwriteDMAMem			(unsigned short * pusPSXMem,int iSize)							{psxspu.DMAWrite(pusPSXMem, iSize);}
	void CALLBACK						MDFNSPUregisterCallback		(void (CALLBACK *callback)(void))								{irqCallback = callback;}
	void CALLBACK						MDFNSPUregisterCDDAVolume	(void (CALLBACK *CDDAVcallback)(unsigned short,unsigned short))	{cddavCallback = CDDAVcallback;}
	long CALLBACK						MDFNSPUtest					(void)															{return 0;}
	long CALLBACK						MDFNSPUconfigure			(void)															{return 0;}
	void CALLBACK						MDFNSPUabout				(void)															{}
	void CALLBACK						MDFNSPUupdate				(void)															{assert(false);}
	long CALLBACK						MDFNSPUinit					(void)															{return 0;}
	long								MDFNSPUopen					(void)															{return 0;}
	long CALLBACK						MDFNSPUclose				(void)															{return 0;}
	long CALLBACK						MDFNSPUshutdown				(void)															{return 0;}
	void CALLBACK						MDFNSPUplayADPCMchannel		(xa_decode_t *xap)												{psxspu.Streamer.FeedXA(xap);}
	void CALLBACK						MDFNSPUplayCDDAchannel		(short *pcm, int nbytes)										{psxspu.Streamer.FeedCDDA(pcm, nbytes);}
	void CALLBACK						MDFNSPUwriteRegister		(unsigned long reg, unsigned short val)							{psxspu.WriteRegister(reg, val);}
	unsigned short CALLBACK				MDFNSPUreadRegister			(unsigned long reg)												{return psxspu.ReadRegister(reg);}
	void CALLBACK						MDFNSPUasync				(uint32_t aCycles)												{psxspu.Process(aCycles);}
	
	long CALLBACK						MDFNSPUfreeze				(uint32_t ulFreezeMode,SPUFreeze_t * pF)
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
