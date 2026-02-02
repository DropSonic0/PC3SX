/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Plugins.h"
#include "spu.h"
#include "PsxCommon.h"
#include "Plugin.h"

#ifdef __WIN32__
#pragma warning(disable:4244)
#endif

#define CheckErr(func) \
    err = SysLibError(); \
    if (err != NULL) { SysMessage ("Error loading %s: %s\n",func,err); return -1; }

#define LoadSym(dest, src, name, checkerr) \
    dest = (src) SysLoadSym(drv, name); if (checkerr == 1) CheckErr(name); \
    if (checkerr == 2) { err = SysLibError(); if (err != NULL) errval = 1; }

static char *err;
static int errval;

extern int ReadPsxPad(int PadNbr);

void *hGPUDriver;

// GPU plugin function pointers
CBGPUupdateLace    GPU_updateLace;
CBGPUinit          GPU_init;
CBGPUshutdown      GPU_shutdown; 
CBGPUconfigure     GPU_configure;
CBGPUtest          GPU_test;
CBGPUabout         GPU_about;
CBGPUopen          GPU_open;
CBGPUclose         GPU_close;
CBGPUreadStatus    GPU_readStatus;
CBGPUreadData      GPU_readData;
CBGPUreadDataMem   GPU_readDataMem;
CBGPUwriteStatus   GPU_writeStatus; 
CBGPUwriteData     GPU_writeData;
CBGPUwriteDataMem  GPU_writeDataMem;
CBGPUdmaChain      GPU_dmaChain;
CBGPUkeypressed    GPU_keypressed;
CBGPUdisplayText   GPU_displayText;
CBGPUmakeSnapshot  GPU_makeSnapshot;
CBGPUfreeze        GPU_freeze;
CBGPUgetScreenPic  GPU_getScreenPic;
CBGPUshowScreenPic GPU_showScreenPic;
CBGPUclearDynarec  GPU_clearDynarec;

// CDR plugin function pointers
CDRinit               CDR_init;
CDRshutdown           CDR_shutdown;
CDRopen               CDR_open;
CDRclose              CDR_close; 
CDRtest               CDR_test;
CDRgetTN              CDR_getTN;
CDRgetTD              CDR_getTD;
CDRreadTrack          CDR_readTrack;
CDRgetBuffer          CDR_getBuffer;
CDRplay               CDR_play;
CDRstop               CDR_stop;
CDRgetStatus          CDR_getStatus;
CDRsetfilename        CDR_setfilename;
CDRgetDriveLetter     CDR_getDriveLetter;
CDRgetBufferSub       CDR_getBufferSub;
CDRconfigure          CDR_configure;
CDRabout              CDR_about;
CDRreadCDDA           CDR_readCDDA;

// SPU plugin function pointers
SPUconfigure        SPU_configure;
SPUabout            SPU_about;
SPUinit             SPU_init;
SPUshutdown         SPU_shutdown;
SPUtest             SPU_test;
SPUopen             SPU_open;
SPUclose            SPU_close;
SPUplaySample       SPU_playSample;
SPUstartChannels1   SPU_startChannels1;
SPUstartChannels2   SPU_startChannels2;
SPUstopChannels1    SPU_stopChannels1;
SPUstopChannels2    SPU_stopChannels2;
SPUputOne           SPU_putOne;
SPUgetOne           SPU_getOne;
SPUsetAddr          SPU_setAddr;
SPUsetPitch         SPU_setPitch;
SPUsetVolumeL       SPU_setVolumeL;
SPUsetVolumeR       SPU_setVolumeR;
SPUwriteRegister    SPU_writeRegister;
SPUreadRegister     SPU_readRegister;
SPUwriteDMA         SPU_writeDMA;
SPUreadDMA          SPU_readDMA;
SPUwriteDMAMem      SPU_writeDMAMem;
SPUreadDMAMem       SPU_readDMAMem;
SPUplayADPCMchannel SPU_playADPCMchannel;
SPUplayCDDAchannel  SPU_playCDDAchannel;
SPUfreeze           SPU_freeze;
SPUregisterCallback SPU_registerCallback;
SPUasync            SPU_async;

// PAD plugin function pointers
PADconfigure        PAD1_configure;
PADabout            PAD1_about;
PADinit             PAD1_init;
PADshutdown         PAD1_shutdown;
PADtest             PAD1_test;
PADopen             PAD1_open;
PADclose            PAD1_close;
PADquery			PAD1_query;
PADreadPort1		PAD1_readPort1;
PADkeypressed		PAD1_keypressed;
PADstartPoll        PAD1_startPoll;
PADpoll             PAD1_poll;
PADsetSensitive     PAD1_setSensitive;

PADconfigure        PAD2_configure;
PADabout            PAD2_about;
PADinit             PAD2_init;
PADshutdown         PAD2_shutdown;
PADtest             PAD2_test;
PADopen             PAD2_open;
PADclose            PAD2_close;
PADquery            PAD2_query;
PADreadPort2		PAD2_readPort2;
PADkeypressed		PAD2_keypressed;
PADstartPoll        PAD2_startPoll;
PADpoll             PAD2_poll;
PADsetSensitive     PAD2_setSensitive;

// NET plugin function pointers
NETinit               NET_init;
NETshutdown           NET_shutdown;
NETclose              NET_close; 
NETtest               NET_test;
NETconfigure          NET_configure;
NETabout              NET_about;
NETpause              NET_pause;
NETresume             NET_resume;
NETqueryPlayer        NET_queryPlayer;
NETsendData           NET_sendData;
NETrecvData           NET_recvData;
NETsendPadData        NET_sendPadData;
NETrecvPadData        NET_recvPadData;
NETsetInfo            NET_setInfo;
NETkeypressed         NET_keypressed;

void ConfigurePlugins();

void CALLBACK GPU__readDataMem(unsigned long *pMem, int iSize) {
	while (iSize > 0) {
		*pMem = GPU_readData();
		iSize--;
		pMem++;
	}		
}

void CALLBACK GPU__writeDataMem(unsigned long *pMem, int iSize) {
	while (iSize > 0) {
		GPU_writeData(*pMem);
		iSize--;
		pMem++;
	}
}

void CALLBACK GPU__displayText(char *pText) {
	SysPrintf("%s\n", pText);
}

void CALLBACK clearDynarec(void) {
	psxCpu->Reset();
}

extern int StatesC;
long CALLBACK GPU__freeze(unsigned long ulGetFreezeData, GPUFreeze_t *pF) {
	pF->ulFreezeVersion = 1;
	if (ulGetFreezeData == 0) {
		int val;

		val = GPU_readStatus();
		val = 0x04000000 | ((val >> 29) & 0x3);
		GPU_writeStatus(0x04000003);
		GPU_writeStatus(0x01000000);
		GPU_writeData(0xa0000000);
		GPU_writeData(0x00000000);
		GPU_writeData(0x02000400);
		GPU_writeDataMem((unsigned long*)pF->psxVRam, 0x100000/4);
		GPU_writeStatus(val);

		val = pF->ulStatus;
		GPU_writeStatus(0x00000000);
		GPU_writeData(0x01000000);
		GPU_writeStatus(0x01000000);
		GPU_writeStatus(0x03000000 | ((val>>24)&0x1));
		GPU_writeStatus(0x04000000 | ((val>>29)&0x3));
		GPU_writeStatus(0x08000000 | ((val>>17)&0x3f) | ((val>>10)&0x40));
		GPU_writeData(0xe1000000 | (val&0x7ff));
		GPU_writeData(0xe6000000 | ((val>>11)&3));

/*		GPU_writeData(0xe3000000 | pF->ulControl[0] & 0xffffff);
		GPU_writeData(0xe4000000 | pF->ulControl[1] & 0xffffff);
		GPU_writeData(0xe5000000 | pF->ulControl[2] & 0xffffff);*/
		return 1;
	}
	if (ulGetFreezeData == 1) {
		int val;

		val = GPU_readStatus();
		val = 0x04000000 | ((val >> 29) & 0x3);
		GPU_writeStatus(0x04000003);
		GPU_writeStatus(0x01000000);
		GPU_writeData(0xc0000000);
		GPU_writeData(0x00000000);
		GPU_writeData(0x02000400);
		GPU_readDataMem((unsigned long*)pF->psxVRam, 0x100000/4);
		GPU_writeStatus(val);

		pF->ulStatus = GPU_readStatus();

/*		GPU_writeStatus(0x10000003);
		pF->ulControl[0] = GPU_readData();
		GPU_writeStatus(0x10000004);
		pF->ulControl[1] = GPU_readData();
		GPU_writeStatus(0x10000005);
		pF->ulControl[2] = GPU_readData();*/
		return 1;
	}
	if(ulGetFreezeData==2) {
		long lSlotNum=*((long *)pF);
		char Text[32];

		sprintf (Text, "*PCSX*: Selected State %ld", lSlotNum+1);
		GPU_displayText(Text);
		return 1;
	}
	return 0;
}

long CALLBACK GPU__configure(void) { return 0; }
long CALLBACK GPU__test(void) { return 0; }
void CALLBACK GPU__about(void) {}
void CALLBACK GPU__makeSnapshot(void) {}
void CALLBACK GPU__keypressed(int key) {}
long CALLBACK GPU__getScreenPic(unsigned char *pMem) { return -1; }
long CALLBACK GPU__showScreenPic(unsigned char *pMem) { return -1; }
void CALLBACK GPU__clearDynarec(void (CALLBACK *callback)(void)) { }

#if defined(__ppc__)
#define PS3LoadGpuSym(dest) \
	GPU_##dest = (CBGPU##dest) GPU##dest;

#define PS3LoadGpuSym1(dest) \
	GPU_##dest = (CBGPU##dest) GPU__##dest;
#else
#define LoadGpuSym1(dest, name) \
	LoadSym(GPU_##dest, GPU##dest, name, 1);

#define LoadGpuSym0(dest, name) \
	LoadSym(GPU_##dest, GPU##dest, name, 0); \
	if (GPU_##dest == NULL) GPU_##dest = (GPU##dest) GPU__##dest;
#endif

extern long CALLBACK GPUinit(void);
extern long CALLBACK GPUopen(void);
extern long CALLBACK GPUshutdown(void);
extern long CALLBACK GPUclose(void);
extern void CALLBACK GPUwriteStatus(unsigned long);
extern void CALLBACK GPUwriteData(unsigned long);
extern void CALLBACK GPUwriteDataMem(unsigned long *, int);
extern unsigned long CALLBACK GPUreadStatus(void);
extern unsigned long CALLBACK GPUreadData(void);
extern void CALLBACK GPUreadDataMem(unsigned long *, int);
extern long CALLBACK GPUdmaChain(unsigned long *,unsigned long);
extern void CALLBACK GPUupdateLace(void);
extern long CALLBACK GPUconfigure(void);
extern long CALLBACK GPUtest(void);
extern void CALLBACK GPUabout(void);
extern void CALLBACK GPUmakeSnapshot(void);

int LoadGPUplugin(char *GPUdll) {
	void *drv;

	hGPUDriver = SysLoadLibrary(GPUdll);
	if (hGPUDriver == NULL) { 
		GPU_configure = NULL;
		SysMessage (_("Could Not Load GPU Plugin %s"), GPUdll); return -1; 
	}
	drv = hGPUDriver;
	SysPrintf("address of init %X\n",GPU_init);
	SysPrintf("address of init2 %X\n",GPUinit);
	GPU_init = (CBGPUinit) GPUinit;
	SysPrintf("address of init2 %X\n",GPU_init);
	
	SysPrintf("start of PS3LoadGpuSym\n");
	PS3LoadGpuSym(open);
	PS3LoadGpuSym(shutdown);
	
	PS3LoadGpuSym(close);
	PS3LoadGpuSym(readData);
	PS3LoadGpuSym(readDataMem);
	PS3LoadGpuSym(readStatus);
	PS3LoadGpuSym(writeData);
	PS3LoadGpuSym(writeDataMem);
	PS3LoadGpuSym(writeStatus);
	PS3LoadGpuSym(dmaChain);
	PS3LoadGpuSym(updateLace);	

	PS3LoadGpuSym1(displayText);	
	PS3LoadGpuSym1(freeze);
	PS3LoadGpuSym1(getScreenPic);
	PS3LoadGpuSym1(showScreenPic);
	PS3LoadGpuSym1(clearDynarec);
	PS3LoadGpuSym1(configure);
	PS3LoadGpuSym1(test);
	PS3LoadGpuSym1(about);
	PS3LoadGpuSym1(makeSnapshot);
	PS3LoadGpuSym1(keypressed);
	
	SysPrintf("end of PS3LoadGpuSym\n");
	
	return 0;
}

void *hCDRDriver;

long CALLBACK CDR__play(unsigned char *sector) { return 0; }
long CALLBACK CDR__stop(void) { return 0; }

long CALLBACK CDR__getStatus(struct CdrStat *stat) {
    if (cdOpenCase) stat->Status = 0x10;
    else stat->Status = 0;
    return 0;
}

char* CALLBACK CDR__getDriveLetter(void) { return NULL; }
unsigned char* CALLBACK CDR__getBufferSub(void) { return NULL; }
long CALLBACK CDR__setfilename(char *filename) { return 0; }
long CALLBACK CDR__readCDDA(unsigned char m, unsigned char s, unsigned char f, unsigned char *buffer) { return -1; }
long CALLBACK CDR__configure(void) { return 0; }
long CALLBACK CDR__test(void) { return 0; }
void CALLBACK CDR__about(void) {}

#if defined(__ppc__)
#define LoadCdrSym0(dest, name) CDR_##dest = (CDR##dest) CDR__##dest;
#define LoadCdrSym1(dest, name) CDR_##dest = (CDR##dest) CDR__##dest;
#else
#define LoadCdrSym1(dest, name) \
	LoadSym(CDR_##dest, CDR##dest, name, 1);

#define LoadCdrSym0(dest, name) \
	LoadSym(CDR_##dest, CDR##dest, name, 0); \
	if (CDR_##dest == NULL) CDR_##dest = (CDR##dest) CDR__##dest;
#endif

int LoadCDRplugin(char *CDRdll) {
	void *drv;

	hCDRDriver = SysLoadLibrary(CDRdll);
	if (hCDRDriver == NULL) { SysMessage ("Could Not load CDR plugin %s\n",CDRdll);  return -1; }
	drv = hCDRDriver;
	LoadCdrSym1(init, "CDRinit");
	LoadCdrSym1(shutdown, "CDRshutdown");
	LoadCdrSym1(open, "CDRopen");
	LoadCdrSym1(close, "CDRclose");
	LoadCdrSym1(getTN, "CDRgetTN");
	LoadCdrSym1(getTD, "CDRgetTD");
	LoadCdrSym1(readTrack, "CDRreadTrack");
	LoadCdrSym1(getBuffer, "CDRgetBuffer");
	LoadCdrSym0(play, "CDRplay");
	LoadCdrSym0(stop, "CDRstop");
	LoadCdrSym0(getStatus, "CDRgetStatus");
	LoadCdrSym0(getDriveLetter, "CDRgetDriveLetter");
	LoadCdrSym0(getBufferSub, "CDRgetBufferSub");
	LoadCdrSym0(configure, "CDRconfigure");
	LoadCdrSym0(test, "CDRtest");
	LoadCdrSym0(about, "CDRabout");
	LoadCdrSym0(readCDDA, "CDRreadCDDA");
	LoadCdrSym0(setfilename, "CDRsetfilename");

	return 0;
}
/*
void *hSPUDriver;

long CALLBACK SPU__configure(void) { return 0; }
void CALLBACK SPU__about(void) {}
long CALLBACK SPU__test(void) { return 0; }

unsigned short regArea[10000];
unsigned short spuCtrl,spuStat,spuIrq;
unsigned long spuAddr;


void CALLBACK SPU__writeRegister(unsigned long add,unsigned short value) { // Old Interface
	unsigned long r=add&0xfff;
	regArea[(r-0xc00)/2] = value;

	if(r>=0x0c00 && r<0x0d80) {
		unsigned char ch=(r>>4)-0xc0;
		switch(r&0x0f) {//switch voices
			case 0:  //left volume
        		SPU_setVolumeL(ch,value);
				return;
			case 2: //right volume
				SPU_setVolumeR(ch,value);
				return;
			case 4:  //frequency
				SPU_setPitch(ch,value);
				return;
			case 6://start address
	            		SPU_setAddr(ch,value);
				return;
     //------------------------------------------------// level
//     			case 8:
//       			s_chan[ch].ADSRX.AttackModeExp  = (val&0x8000)?TRUE:FALSE;
//       			s_chan[ch].ADSRX.AttackRate     = (float)((val>>8) & 0x007f)*1000.0f/240.0f;
//       			s_chan[ch].ADSRX.DecayRate      = (float)((val>>4) & 0x000f)*1000.0f/240.0f;
//      			s_chan[ch].ADSRX.SustainLevel   = (float)((val)    & 0x000f);

//       			return;
//     			case 10:
//       			s_chan[ch].ADSRX.SustainModeExp = (val&0x8000)?TRUE:FALSE;
//       			s_chan[ch].ADSRX.ReleaseModeExp = (val&0x0020)?TRUE:FALSE;
//       			s_chan[ch].ADSRX.SustainRate    = ((float)((val>>6) & 0x007f))*R_SUSTAIN;
//       			s_chan[ch].ADSRX.ReleaseRate    = ((float)((val)    & 0x001f))*R_RELEASE;
//       			if(val & 0x4000) s_chan[ch].ADSRX.SustainModeDec=-1.0f;
//       			else             s_chan[ch].ADSRX.SustainModeDec=1.0f;
//       			return;
//     			case 12:
//       			return;
//     			case 14:                                    
//       			s_chan[ch].pRepeat=spuMemC+((unsigned long) val<<3);
//       			return;
		}
    		return;
	}

	switch(r) {
		case H_SPUaddr://SPU-memory address
    			spuAddr = (unsigned long) value<<3;
		//	spuAddr=value * 8;
    			return;
		case H_SPUdata://DATA to SPU
//      		spuMem[spuAddr/2] = value;
//         		spuAddr+=2;
//        		if(spuAddr>0x7ffff) spuAddr=0;
			SPU_putOne(spuAddr,value);
    			spuAddr+=2;
    			return;
		case H_SPUctrl://SPU control 1
    			spuCtrl=value;
    			return;
		case H_SPUstat://SPU status
                        spuStat=value & 0xf800;
    			return;
		case H_SPUirqAddr://SPU irq
    			spuIrq = value;
    			return;
		case H_SPUon1://start sound play channels 0-16
		        SPU_startChannels1(value);
    			return;
		case H_SPUon2://start sound play channels 16-24
    			SPU_startChannels2(value);
    			return;
		case H_SPUoff1://stop sound play channels 0-16
    			SPU_stopChannels1(value);
    			return;
		case H_SPUoff2://stop sound play channels 16-24
    			SPU_stopChannels2(value);
    			return;		
	}
}

unsigned short CALLBACK SPU__readRegister(unsigned long add) {
	switch(add&0xfff) {// Old Interface
		case H_SPUctrl://spu control
    			return spuCtrl;
		case H_SPUstat://spu status
    			return spuStat;
		case H_SPUaddr://SPU-memory address
                         return (unsigned short)(spuAddr>>3);
		case H_SPUdata://DATA to SPU
    			spuAddr+=2;
//        		if(spuAddr>0x7ffff) spuAddr=0;
//        		return spuMem[spuAddr/2];
			return SPU_getOne(spuAddr);
		case H_SPUirqAddr://spu irq
    			return spuIrq;
		//case H_SPUIsOn1:
    			//return IsSoundOn(0,16);
		//case H_SPUIsOn2:
    			//return IsSoundOn(16,24);
	}
	return regArea[((add&0xfff)-0xc00)/2];
}

void CALLBACK SPU__writeDMA(unsigned short val) {
	SPU_putOne(spuAddr, val);
	spuAddr += 2;
	if (spuAddr > 0x7ffff) spuAddr = 0;
}

unsigned short CALLBACK SPU__readDMA(void) {
	unsigned short tmp = SPU_getOne(spuAddr);
	spuAddr += 2;
	if (spuAddr > 0x7ffff) spuAddr = 0;
	return tmp;
}

void CALLBACK SPU__writeDMAMem(unsigned short *pMem, int iSize) {
	while (iSize > 0) {
		SPU_writeDMA(*pMem);
		iSize--;
		pMem++;
	}		
}

void CALLBACK SPU__readDMAMem(unsigned short *pMem, int iSize) ;
void CALLBACK SPU__writeDMAMem(unsigned short *pMem, int iSize);
unsigned short CALLBACK SPU__readDMA(void);
void CALLBACK SPU__writeDMA(unsigned short val);
unsigned short CALLBACK SPU__readRegister(unsigned long add);
void CALLBACK SPU__writeRegister(unsigned long add,unsigned short value);

void CALLBACK SPU__readDMAMem(unsigned short *pMem, int iSize) {
	while (iSize > 0) {
		*pMem = SPU_readDMA();
		iSize--;
		pMem++;
	}		
}

void CALLBACK SPU__playADPCMchannel(xa_decode_t *xap) {}

long CALLBACK SPU__freeze(unsigned long ulFreezeMode, SPUFreeze_t *pF) {
	if (ulFreezeMode == 2) {
		memset(pF, 0, 16);
		strcpy((char *)pF->PluginName, "Pcsx");
		pF->PluginVersion = 1;
		pF->Size = 0x200 + 0x80000 + 16 + sizeof(xa_decode_t);

		return 1;
	}
	if (ulFreezeMode == 1) {
		unsigned long addr;
		unsigned short val;

		val = SPU_readRegister(0x1f801da6);
		SPU_writeRegister(0x1f801da6, 0);
		SPU_readDMAMem((unsigned short *)pF->SPURam, 0x80000/2);
		SPU_writeRegister(0x1f801da6, val);

		for (addr = 0x1f801c00; addr < 0x1f801e00; addr+=2) {
			if (addr == 0x1f801da8) { pF->SPUPorts[addr - 0x1f801c00] = 0; continue; }
			pF->SPUPorts[addr - 0x1f801c00] = SPU_readRegister(addr);
		}

		return 1;
	}
	if (ulFreezeMode == 0) {
		unsigned long addr;
		unsigned short val;
		unsigned short *regs = (unsigned short *)pF->SPUPorts;

		val = SPU_readRegister(0x1f801da6);
		SPU_writeRegister(0x1f801da6, 0);
		SPU_writeDMAMem((unsigned short *)pF->SPURam, 0x80000/2);
		SPU_writeRegister(0x1f801da6, val);

		for (addr = 0x1f801c00; addr < 0x1f801e00; addr+=2) {
			if (addr == 0x1f801da8) { regs++; continue; }
			SPU_writeRegister(addr, *(regs++));
		}

		return 1;
	}

	return 0;
}

void CALLBACK SPU__registerCallback(void (CALLBACK *callback)(void)) {}
void CALLBACK SPU__update(void) {}
void SPU_d_async(unsigned long v);

#if defined(__ppc__)
#define LoadSpuSymFAKE(dest, name) \
	SPU_##dest = (SPU##dest) SPU__##dest;

#define LoadSpuSymFAKE1(dest, name) \
	SPU_##dest = (SPU##dest) SPU_d_##dest;
#else
#define LoadSpuSym1(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, 1);

#define LoadSpuSym2(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, 2);

#define LoadSpuSym0(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, 0); \
	if (SPU_##dest == NULL) SPU_##dest = (SPU##dest) SPU__##dest;

#define LoadSpuSymE(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, errval); \
	if (SPU_##dest == NULL) SPU_##dest = (SPU##dest) SPU__##dest;
#endif

int LoadSPUplugin(char *SPUdll) {
	void *drv;

	hSPUDriver = SysLoadLibrary(SPUdll);
	if (hSPUDriver == NULL) {
		SPU_configure = NULL;
		SysMessage (_("Could not open SPU plugin %s"), SPUdll); return -1;
	}
	drv = hSPUDriver;
	//LoadSpuSymFAKE
	LoadSpuSymFAKE1(init, "SPUinit");
	LoadSpuSymFAKE1(shutdown, "SPUshutdown");
	LoadSpuSymFAKE1(open, "SPUopen");
	LoadSpuSymFAKE1(close, "SPUclose");
	LoadSpuSymFAKE(configure, "SPUconfigure");
	LoadSpuSymFAKE(about, "SPUabout");
	LoadSpuSymFAKE(test, "SPUtest");
	errval = 0;
	LoadSpuSymFAKE1(startChannels1, "SPUstartChannels1");
	LoadSpuSymFAKE1(startChannels2, "SPUstartChannels2");
	LoadSpuSymFAKE1(stopChannels1, "SPUstopChannels1");
	LoadSpuSymFAKE1(stopChannels2, "SPUstopChannels2");
	LoadSpuSymFAKE1(putOne, "SPUputOne");
	LoadSpuSymFAKE1(getOne, "SPUgetOne");
	LoadSpuSymFAKE1(setAddr, "SPUsetAddr");
	LoadSpuSymFAKE1(setPitch, "SPUsetPitch");
	LoadSpuSymFAKE1(setVolumeL, "SPUsetVolumeL");
	LoadSpuSymFAKE1(setVolumeR, "SPUsetVolumeR");
	LoadSpuSymFAKE(writeRegister, "SPUwriteRegister");
	LoadSpuSymFAKE(readRegister, "SPUreadRegister");		
	LoadSpuSymFAKE(writeDMA, "SPUwriteDMA");
	LoadSpuSymFAKE(readDMA, "SPUreadDMA");
	LoadSpuSymFAKE(writeDMAMem, "SPUwriteDMAMem");
	LoadSpuSymFAKE(readDMAMem, "SPUreadDMAMem");
	LoadSpuSymFAKE(playADPCMchannel, "SPUplayADPCMchannel");
	LoadSpuSymFAKE(freeze, "SPUfreeze");
	LoadSpuSymFAKE(registerCallback, "SPUregisterCallback");
	LoadSpuSymFAKE1(async, "SPUasync");

	return 0;
}
*/

void *hSPUDriver;

extern long CALLBACK SPUconfigure(void);
extern void CALLBACK SPUabout(void);
extern long CALLBACK SPUtest(void);
extern long CALLBACK SPUinit(void);
extern long CALLBACK SPUshutdown(void);
extern long CALLBACK SPUopen(void);
extern long CALLBACK SPUclose(void);
extern void CALLBACK SPUwriteRegister(unsigned long reg, unsigned short val);
extern unsigned short CALLBACK SPUreadRegister(unsigned long reg);
extern void CALLBACK SPUwriteDMA(unsigned short val);
extern unsigned short CALLBACK SPUreadDMA(void);
extern void CALLBACK SPUwriteDMAMem(unsigned short * pusPSXMem,int iSize);
extern void CALLBACK SPUreadDMAMem(unsigned short * pusPSXMem,int iSize);
extern void CALLBACK SPUplayADPCMchannel(xa_decode_t *xap);
extern void CALLBACK SPUplayCDDAchannel(short *pcm, int nbytes);
extern void CALLBACK SPUregisterCallback(void (CALLBACK *callback)(void));
extern void CALLBACK SPUregisterCDDAVolume(void (CALLBACK *CDDAVcallback)(unsigned short,unsigned short));
extern void CALLBACK SPUasync(unsigned long cycles);
extern long CALLBACK SPUfreeze(unsigned long ulFreezeMode, SPUFreeze_t *pF);

#define LoadSpuSym(dest) \
	SPU_##dest = (SPU##dest) SPU##dest;

int LoadSPUplugin(char *SPUdll) {
	hSPUDriver = SysLoadLibrary(SPUdll);
	if (hSPUDriver == NULL) {
		SPU_configure = NULL;
		SysPrintf ("Could not open SPU plugin %s\n", SPUdll); return -1;
	}

	LoadSpuSym(init);
	LoadSpuSym(shutdown);
	LoadSpuSym(open);
	LoadSpuSym(close);
	LoadSpuSym(configure);
	LoadSpuSym(about);
	LoadSpuSym(test);
	LoadSpuSym(writeRegister);
	LoadSpuSym(readRegister);
	LoadSpuSym(writeDMA);
	LoadSpuSym(readDMA);
	LoadSpuSym(writeDMAMem);
	LoadSpuSym(readDMAMem);
	LoadSpuSym(playADPCMchannel);
	LoadSpuSym(playCDDAchannel);
	LoadSpuSym(freeze);
	LoadSpuSym(async);
	LoadSpuSym(registerCallback);

	return 0;
}

void *hPAD1Driver;
void *hPAD2Driver;

static unsigned char buf[256];
unsigned char stdpar[10] = { 0x00, 0x41, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char mousepar[8] = { 0x00, 0x12, 0x5a, 0xff, 0xff, 0xff, 0xff };
unsigned char analogpar[9] = { 0x00, 0xff, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static int bufcount, bufc;

PadDataS padd1, padd2;

unsigned char _PADstartPoll(PadDataS *pad) {
	bufc = 0;

	switch (pad->controllerType)
	{
		case PSE_PAD_TYPE_MOUSE:
			mousepar[3] = pad->buttonStatus & 0xff;
			mousepar[4] = pad->buttonStatus >> 8;
			mousepar[5] = pad->moveX;
			mousepar[6] = pad->moveY;

			memcpy(buf, mousepar, 7);
			bufcount = 6;
			break;
		case PSE_PAD_TYPE_NEGCON: // npc101/npc104(slph00001/slph00069)
			analogpar[1] = 0x23;
			analogpar[3] = pad->buttonStatus & 0xff;
			analogpar[4] = pad->buttonStatus >> 8;
			analogpar[5] = pad->rightJoyX;
			analogpar[6] = pad->rightJoyY;
			analogpar[7] = pad->leftJoyX;
			analogpar[8] = pad->leftJoyY;

			memcpy(buf, analogpar, 9);
			bufcount = 8;
			break;
		case PSE_PAD_TYPE_ANALOGPAD: // scph1150
			analogpar[1] = 0x73;
			analogpar[3] = pad->buttonStatus & 0xff;
			analogpar[4] = pad->buttonStatus >> 8;
			analogpar[5] = pad->rightJoyX;
			analogpar[6] = pad->rightJoyY;
			analogpar[7] = pad->leftJoyX;
			analogpar[8] = pad->leftJoyY;

			memcpy(buf, analogpar, 9);
			bufcount = 8;
			break;
		case PSE_PAD_TYPE_ANALOGJOY: // scph1110
			analogpar[1] = 0x53;
			analogpar[3] = pad->buttonStatus & 0xff;
			analogpar[4] = pad->buttonStatus >> 8;
			analogpar[5] = pad->rightJoyX;
			analogpar[6] = pad->rightJoyY;
			analogpar[7] = pad->leftJoyX;
			analogpar[8] = pad->leftJoyY;

			memcpy(buf, analogpar, 9);
			bufcount = 8;
			break;
		case PSE_PAD_TYPE_STANDARD:
		default:
			stdpar[3] = pad->buttonStatus & 0xff;
			stdpar[4] = pad->buttonStatus >> 8;

			memcpy(buf, stdpar, 5);
			bufcount = 4;
			break;
	}

	return buf[bufc++];
}

unsigned char _PADpoll(unsigned char value) {
	if (bufc > bufcount) return 0;
	return buf[bufc++];
}

unsigned char CALLBACK PAD1__startPoll(int pad) {
	PadDataS padd;

	PAD1_readPort1(&padd);

	return _PADstartPoll(&padd);
}

unsigned char CALLBACK PAD1__poll(unsigned char value) {
	return _PADpoll(value);
}


long CALLBACK PAD1__configure(void) { return 0; }
void CALLBACK PAD1__about(void) {}
long CALLBACK PAD1__test(void) { return 0; }
long CALLBACK PAD1__query(void) { return 3; }
long CALLBACK PAD1__keypressed() { return 0; }

#if defined(__ppc__)
#define LoadPad1Sym0(dest, name) PAD1_##dest = (PAD##dest) PAD1__##dest;
#define LoadPad1Sym1(dest, name) PAD1_##dest = (PAD##dest) PAD__##dest;
#else
#define LoadPad1Sym1(dest, name) \
	LoadSym(PAD1_##dest, PAD##dest, name, 1);

#define LoadPad1Sym0(dest, name) \
	LoadSym(PAD1_##dest, PAD##dest, name, 0); \
	if (PAD1_##dest == NULL) PAD1_##dest = (PAD##dest) PAD1__##dest;
#endif

int LoadPAD1plugin(char *PAD1dll) {
	void *drv;

	hPAD1Driver = SysLoadLibrary(PAD1dll);
	if (hPAD1Driver == NULL) { SysMessage ("Could Not load PAD1 plugin %s\n",PAD1dll);  return -1; }
	drv = hPAD1Driver;
	LoadPad1Sym1(init, "PADinit");
	LoadPad1Sym1(shutdown, "PADshutdown");
	LoadPad1Sym1(open, "PADopen");
	LoadPad1Sym1(close, "PADclose");
	LoadPad1Sym0(query, "PADquery");
	LoadPad1Sym1(readPort1, "PADreadPort1");
	LoadPad1Sym0(configure, "PADconfigure");
	LoadPad1Sym0(test, "PADtest");
	LoadPad1Sym0(about, "PADabout");
	LoadPad1Sym0(keypressed, "PADkeypressed");
	LoadPad1Sym0(startPoll, "PADstartPoll");
	LoadPad1Sym0(poll, "PADpoll");

	return 0;
}

unsigned char CALLBACK PAD2__startPoll(int pad) {
	PadDataS padd;

	PAD2_readPort2(&padd);
	
	return _PADstartPoll(&padd);
}

unsigned char CALLBACK PAD2__poll(unsigned char value) {
	return _PADpoll(value);
}

long CALLBACK PAD2__configure(void) { return 0; }
void CALLBACK PAD2__about(void) {}
long CALLBACK PAD2__test(void) { return 0; }
long CALLBACK PAD2__query(void) { return 3; }
long CALLBACK PAD2__keypressed() { return 0; }

#if defined(__ppc__)
#define LoadPad2Sym0(dest, name) PAD2_##dest = (PAD##dest) PAD2__##dest;
#define LoadPad2Sym1(dest, name) PAD2_##dest = (PAD##dest) PAD__##dest;
#else
#define LoadPad2Sym1(dest, name) \
	LoadSym(PAD2_##dest, PAD##dest, name, 1);

#define LoadPad2Sym0(dest, name) \
	LoadSym(PAD2_##dest, PAD##dest, name, 0); \
	if (PAD2_##dest == NULL) PAD2_##dest = (PAD##dest) PAD2__##dest;
#endif

int LoadPAD2plugin(char *PAD2dll) {
	void *drv;

	hPAD2Driver = SysLoadLibrary(PAD2dll);
	if (hPAD2Driver == NULL) { SysMessage ("Could Not load PAD plugin %s\n",PAD2dll);  return -1; }
	drv = hPAD2Driver;
	LoadPad2Sym1(init, "PADinit");
	LoadPad2Sym1(shutdown, "PADshutdown");
	LoadPad2Sym1(open, "PADopen");
	LoadPad2Sym1(close, "PADclose");
	LoadPad2Sym0(query, "PADquery");
	LoadPad2Sym1(readPort2, "PADreadPort2");
	LoadPad2Sym0(configure, "PADconfigure");
	LoadPad2Sym0(test, "PADtest");
	LoadPad2Sym0(about, "PADabout");
	LoadPad2Sym0(keypressed, "PADkeypressed");
	LoadPad2Sym0(startPoll, "PADstartPoll");
	LoadPad2Sym0(poll, "PADpoll");

	return 0;
}


/*long PAD2__readBuffer() {
	return PAD2_readBuffer();
}*/

int LoadPlugins() {
	int ret;
	char Plugin[256];

	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Cdr);
	if (LoadCDRplugin(Plugin) == -1) return -1;
	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Gpu);
	if (LoadGPUplugin(Plugin) == -1) return -1;
	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Spu);
	if (LoadSPUplugin(Plugin) == -1) return -1;

	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Pad1);
	if (LoadPAD1plugin(Plugin) == -1) return -1;
	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Pad2);
	if (LoadPAD2plugin(Plugin) == -1) return -1;
	ret = CDR_init();
	if (ret != 0) { SysMessage ("CDRinit error : %d\n",ret); return -1; }
	ret = GPU_init();
	if (ret != 0) { SysMessage ("GPUinit error : %d\n",ret); return -1; }
	ret = SPU_init();
	if (ret != 0) { SysMessage ("SPUinit error : %d\n",ret); return -1; }
	ret = PAD1_init(1);
	if (ret != 0) { SysMessage ("PAD1init error : %d\n",ret); return -1; }
	ret = PAD2_init(2);
	if (ret != 0) { SysMessage ("PAD2init error : %d\n",ret); return -1; }

	return 0;
}

const char *GetIsoFile(void) {
	return Config.Cdr;
}

void ReleasePlugins() {
	if (hCDRDriver  == NULL || hGPUDriver  == NULL || hSPUDriver == NULL ||
		hPAD1Driver == NULL || hPAD2Driver == NULL) return;
	CDR_shutdown();
	GPU_shutdown();
	SPU_shutdown();
	PAD1_shutdown();
	PAD2_shutdown();
	SysCloseLibrary(hCDRDriver); hCDRDriver = NULL;
	SysCloseLibrary(hGPUDriver); hGPUDriver = NULL;
	SysCloseLibrary(hSPUDriver); hSPUDriver = NULL;
	SysCloseLibrary(hPAD1Driver); hPAD1Driver = NULL;
	SysCloseLibrary(hPAD2Driver); hPAD2Driver = NULL;
}
