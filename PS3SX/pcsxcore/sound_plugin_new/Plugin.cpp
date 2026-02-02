#include "stdafx.h"
#include "SPU.h"
#include "Envelope.h"
#include "ps3audio.h"

static PSX::SPU::SPU psxspu;

typedef struct
{
 char          szSPUName[8];
 uint32_t ulFreezeVersion;
 uint32_t ulFreezeSize;
 unsigned char cSPUPort[0x200];
 unsigned char cSPURam[0x80000];
 xa_decode_t   xaS;
} SPUFreeze_t;

void SoundFeedSample(uint16_t left, uint16_t right)
{
	uint16_t frame[2] = {left, right};
	cellAudioPortWrite((const audio_input_t*)frame, 2);
}

extern "C"
{
	unsigned short CALLBACK				pkSPUreadDMA				(void)															{assert(false); return 0;}
	void CALLBACK						pkSPUreadDMAMem			(unsigned short * pusPSXMem,int iSize)							{psxspu.DMARead(pusPSXMem, iSize);}
	void CALLBACK						pkSPUwriteDMA				(unsigned short val)											{assert(false);}
	void CALLBACK						pkSPUwriteDMAMem			(unsigned short * pusPSXMem,int iSize)							{psxspu.DMAWrite(pusPSXMem, iSize);}
	void CALLBACK						pkSPUregisterCallback		(void (CALLBACK *callback)(void))								{irqCallback = callback;}
	void CALLBACK						pkSPUregisterCDDAVolume	(void (CALLBACK *CDDAVcallback)(unsigned short,unsigned short))	{cddavCallback = CDDAVcallback;}
	long CALLBACK						pkSPUtest					(void)															{return 0;}
	long CALLBACK						pkSPUconfigure			(void)															{return 0;}
	void CALLBACK						pkSPUabout				(void)															{}
	void CALLBACK						pkSPUupdate				(void)															{assert(false);}
	long CALLBACK						pkSPUinit					(void)															{return 0;}
	long CALLBACK						pkSPUopen					(void)															{return 0;}
	long CALLBACK						pkSPUclose				(void)															{return 0;}
	long CALLBACK						pkSPUshutdown				(void)															{return 0;}
	void CALLBACK						pkSPUplayADPCMchannel		(xa_decode_t *xap)												{psxspu.Streamer.FeedXA(xap);}
	void CALLBACK						pkSPUplayCDDAchannel		(short *pcm, int nbytes)										{psxspu.Streamer.FeedCDDA(pcm, nbytes);}
	void CALLBACK						pkSPUwriteRegister		(unsigned long reg, unsigned short val)							{psxspu.WriteRegister(reg, val);}
	unsigned short CALLBACK				pkSPUreadRegister			(unsigned long reg)												{return psxspu.ReadRegister(reg);}
	void CALLBACK						pkSPUasync				(unsigned long aCycles)											{psxspu.Process(aCycles);}
	long CALLBACK						pkSPUfreeze				(unsigned long ulFreezeMode,SPUFreeze_t * pF)					{return -1;}
}
