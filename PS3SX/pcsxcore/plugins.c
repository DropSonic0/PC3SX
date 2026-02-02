/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
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

void ConfigurePlugins(void);

static void CALLBACK GPU__readDataMem(unsigned long *pMem, int iSize);
static void CALLBACK GPU__writeDataMem(unsigned long *pMem, int iSize);
static void CALLBACK GPU__displayText(char *pText);
static long CALLBACK GPU__freeze(unsigned long ulGetFreezeData, GPUFreeze_t *pF);
static long CALLBACK GPU__configure(void);
static long CALLBACK GPU__test(void);
static void CALLBACK GPU__about(void);
static void CALLBACK GPU__makeSnapshot(void);
static void CALLBACK GPU__keypressed(int key);
static long CALLBACK GPU__getScreenPic(unsigned char *pMem);
static long CALLBACK GPU__showScreenPic(unsigned char *pMem);
static void CALLBACK GPU__clearDynarec(void (CALLBACK *callback)(void));

long CALLBACK CDR__play(unsigned char *sector);
long CALLBACK CDR__stop(void);
long CALLBACK CDR__getStatus(struct CdrStat *stat);
char* CALLBACK CDR__getDriveLetter(void);
unsigned char* CALLBACK CDR__getBufferSub(void);
long CALLBACK CDR__setfilename(char *filename);
long CALLBACK CDR__readCDDA(unsigned char m, unsigned char s, unsigned char f, unsigned char *buffer);
long CALLBACK CDR__configure(void);
long CALLBACK CDR__test(void);
void CALLBACK CDR__about(void);

static unsigned char _PADstartPoll(PadDataS *pad);
static unsigned char _PADpoll(unsigned char value);

static unsigned char CALLBACK PAD1__startPoll(int pad);
static unsigned char CALLBACK PAD1__poll(unsigned char value);
static long CALLBACK PAD1__configure(void);
static void CALLBACK PAD1__about(void);
static long CALLBACK PAD1__test(void);
static long CALLBACK PAD1__query(void);
static long CALLBACK PAD1__keypressed(void);

static unsigned char CALLBACK PAD2__startPoll(int pad);
static unsigned char CALLBACK PAD2__poll(unsigned char value);
static long CALLBACK PAD2__configure(void);
static void CALLBACK PAD2__about(void);
static long CALLBACK PAD2__test(void);
static long CALLBACK PAD2__query(void);
static long CALLBACK PAD2__keypressed(void);

static void CALLBACK GPU__readDataMem(unsigned long *pMem, int iSize) {
	while (iSize > 0) {
		*pMem = GPU_readData();
		iSize--;
		pMem++;
	}		
}

static void CALLBACK GPU__writeDataMem(unsigned long *pMem, int iSize) {
	while (iSize > 0) {
		GPU_writeData(*pMem);
		iSize--;
		pMem++;
	}
}

static void CALLBACK GPU__displayText(char *pText) {
	SysPrintf("%s\n", pText);
}

void CALLBACK clearDynarec(void) {
	psxCpu->Reset();
}

static long CALLBACK GPU__freeze(unsigned long ulGetFreezeData, GPUFreeze_t *pF) {
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

static long CALLBACK GPU__configure(void) { return 0; }
static long CALLBACK GPU__test(void) { return 0; }
static void CALLBACK GPU__about(void) {}
static void CALLBACK GPU__makeSnapshot(void) {}
static void CALLBACK GPU__keypressed(int key) { (void)key; }
static long CALLBACK GPU__getScreenPic(unsigned char *pMem) { (void)pMem; return -1; }
static long CALLBACK GPU__showScreenPic(unsigned char *pMem) { (void)pMem; return -1; }
static void CALLBACK GPU__clearDynarec(void (CALLBACK *callback)(void)) { (void)callback; }

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
	(void)GPUdll;
	void *drv;

	hGPUDriver = SysLoadLibrary(GPUdll);
	if (hGPUDriver == NULL) { 
		GPU_configure = (CBGPUconfigure) GPU__configure;
		SysMessage ("Could Not Load GPU Plugin %s", GPUdll); return -1;
	}
	drv = hGPUDriver;
	(void)drv;
	SysPrintf("address of init %X\n", (unsigned int)GPU_init);
	SysPrintf("address of init2 %X\n", (unsigned int)GPUinit);
	GPU_init = (CBGPUinit) GPUinit;
	SysPrintf("address of init2 %X\n", (unsigned int)GPU_init);
	
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

long CALLBACK CDR__play(unsigned char *sector) { (void)sector; return 0; }
long CALLBACK CDR__stop(void) { return 0; }

long CALLBACK CDR__getStatus(struct CdrStat *stat) {
    if (cdOpenCase) stat->Status = 0x10;
    else stat->Status = 0;
    return 0;
}

char* CALLBACK CDR__getDriveLetter(void) { return NULL; }
unsigned char* CALLBACK CDR__getBufferSub(void) { return NULL; }
long CALLBACK CDR__setfilename(char *filename) { (void)filename; return 0; }
long CALLBACK CDR__readCDDA(unsigned char m, unsigned char s, unsigned char f, unsigned char *buffer) { (void)m; (void)s; (void)f; (void)buffer; return -1; }
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
	(void)CDRdll;
	void *drv;

	hCDRDriver = SysLoadLibrary(CDRdll);
	if (hCDRDriver == NULL) { SysMessage ("Could Not load CDR plugin %s\n",CDRdll);  return -1; }
	drv = hCDRDriver;
	(void)drv;
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

void *hSPUDriver;

extern long CALLBACK pkSPUconfigure(void);
extern void CALLBACK pkSPUabout(void);
extern long CALLBACK pkSPUtest(void);
extern long CALLBACK pkSPUinit(void);
extern long CALLBACK pkSPUshutdown(void);
extern long CALLBACK pkSPUopen(void);
extern long CALLBACK pkSPUclose(void);
extern void CALLBACK pkSPUwriteRegister(unsigned long reg, unsigned short val);
extern unsigned short CALLBACK pkSPUreadRegister(unsigned long reg);
extern void CALLBACK pkSPUwriteDMA(unsigned short val);
extern unsigned short CALLBACK pkSPUreadDMA(void);
extern void CALLBACK pkSPUwriteDMAMem(unsigned short * pusPSXMem,int iSize);
extern void CALLBACK pkSPUreadDMAMem(unsigned short * pusPSXMem,int iSize);
extern void CALLBACK pkSPUplayADPCMchannel(xa_decode_t *xap);
extern void CALLBACK pkSPUplayCDDAchannel(short *pcm, int nbytes);
extern void CALLBACK pkSPUregisterCallback(void (CALLBACK *callback)(void));
extern void CALLBACK pkSPUasync(unsigned long cycles);
extern long CALLBACK pkSPUfreeze(unsigned long ulFreezeMode, SPUFreeze_t *pF);

#define LoadSpuSym(dest, func) \
	SPU_##dest = (SPU##dest) pkSPU##func;

int LoadSPUplugin(char *SPUdll) {
	(void)SPUdll;
	hSPUDriver = SysLoadLibrary(SPUdll);
	if (hSPUDriver == NULL) {
		SPU_configure = (SPUconfigure) pkSPUconfigure;
		SysPrintf ("Could not open SPU plugin %s\n", SPUdll); return -1;
	}

	LoadSpuSym(init, init);
	LoadSpuSym(shutdown, shutdown);
	LoadSpuSym(open, open);
	LoadSpuSym(close, close);
	LoadSpuSym(configure, configure);
	LoadSpuSym(about, about);
	LoadSpuSym(test, test);
	LoadSpuSym(writeRegister, writeRegister);
	LoadSpuSym(readRegister, readRegister);
	LoadSpuSym(writeDMA, writeDMA);
	LoadSpuSym(readDMA, readDMA);
	LoadSpuSym(writeDMAMem, writeDMAMem);
	LoadSpuSym(readDMAMem, readDMAMem);
	LoadSpuSym(playADPCMchannel, playADPCMchannel);
	LoadSpuSym(playCDDAchannel, playCDDAchannel);
	LoadSpuSym(freeze, freeze);
	LoadSpuSym(async, async);
	LoadSpuSym(registerCallback, registerCallback);

	return 0;
}

void *hPAD1Driver;
void *hPAD2Driver;

static unsigned char buf[256];
static unsigned char stdpar[10] = { 0x00, 0x41, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
static unsigned char mousepar[8] = { 0x00, 0x12, 0x5a, 0xff, 0xff, 0xff, 0xff };
static unsigned char analogpar[9] = { 0x00, 0xff, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static int bufcount, bufc;

PadDataS padd1, padd2;

static unsigned char _PADstartPoll(PadDataS *pad) {
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

static unsigned char _PADpoll(unsigned char value) {
	(void)value;
	if (bufc > bufcount) return 0;
	return buf[bufc++];
}

static unsigned char CALLBACK PAD1__startPoll(int pad) {
	(void)pad;
	PadDataS padd;

	PAD1_readPort1(&padd);

	return _PADstartPoll(&padd);
}

static unsigned char CALLBACK PAD1__poll(unsigned char value) {
	return _PADpoll(value);
}


static long CALLBACK PAD1__configure(void) { return 0; }
static void CALLBACK PAD1__about(void) {}
static long CALLBACK PAD1__test(void) { return 0; }
static long CALLBACK PAD1__query(void) { return 3; }
static long CALLBACK PAD1__keypressed(void) { return 0; }

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
	(void)PAD1dll;
	void *drv;

	hPAD1Driver = SysLoadLibrary(PAD1dll);
	if (hPAD1Driver == NULL) { SysMessage ("Could Not load PAD1 plugin %s\n",PAD1dll);  return -1; }
	drv = hPAD1Driver;
	(void)drv;
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

static unsigned char CALLBACK PAD2__startPoll(int pad) {
	(void)pad;
	PadDataS padd;

	PAD2_readPort2(&padd);
	
	return _PADstartPoll(&padd);
}

static unsigned char CALLBACK PAD2__poll(unsigned char value) {
	return _PADpoll(value);
}

static long CALLBACK PAD2__configure(void) { return 0; }
static void CALLBACK PAD2__about(void) {}
static long CALLBACK PAD2__test(void) { return 0; }
static long CALLBACK PAD2__query(void) { return 3; }
static long CALLBACK PAD2__keypressed(void) { return 0; }

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
	(void)PAD2dll;
	void *drv;

	hPAD2Driver = SysLoadLibrary(PAD2dll);
	if (hPAD2Driver == NULL) { SysMessage ("Could Not load PAD plugin %s\n",PAD2dll);  return -1; }
	drv = hPAD2Driver;
	(void)drv;
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

int LoadPlugins(void) {
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

void ReleasePlugins(void) {
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
