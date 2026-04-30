#include "stdafx.h"
#include "SPU.h"
#include "Envelope.h"

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
	long CALLBACK						SPUfreeze				(uint32_t ulFreezeMode,SPUFreeze_t * pF)						{return -1;}
}
